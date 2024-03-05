/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IDT.h>
#include <Kernel/Devices/ATA.h>
#include <Kernel/Devices/PATADisk.h>
#include <Kernel/IO.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>

Spinlock PATADisk::s_lock {};
WaitingStatus PATADisk::s_waiting_status {};

UniquePtr<PATADisk> PATADisk::create(Channel channel, Type type)
{
    Bus::PCI::Address ide_controller_address = {};
    Bus::PCI::ID ide_controller_id = {};

    Bus::PCI::enumerate_devices([&](Bus::PCI::Address address, Bus::PCI::ID id, u16 type) {
        if (type == PCI_TYPE_IDE_CONTROLLER) {
            ide_controller_address = address;
            ide_controller_id = id;
            return;
        }
    });

    if (ide_controller_address.bus == 0 && ide_controller_address.slot == 0 && ide_controller_address.function == 0) {
        return nullptr;
    }

    dbgprintf("PATADisk", "Creating '%s' disk using PCI device %x:%x\n", channel == Primary ? "Primary" : "Secondary", ide_controller_id.vendor, ide_controller_id.device);
    UniquePtr<PATADisk> disk = make_unique_ptr<PATADisk>(ide_controller_address, channel, type);

    IO::outb(disk->m_io_base + ATA_REG_HDDEVSEL, 0xA0 | (type == Slave ? 0x10 : 0x00));
    IO::outb(disk->m_io_base + ATA_REG_SECCOUNT0, 0);
    IO::outb(disk->m_io_base + ATA_REG_LBA0, 0);
    IO::outb(disk->m_io_base + ATA_REG_LBA1, 0);
    IO::outb(disk->m_io_base + ATA_REG_LBA2, 0);

    IO::outb(disk->m_io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    u32 status = 0;
    while ((status = IO::inb(disk->m_control_base)) & ATA_SR_BSY)
        ;

    if (status == 0) {
        return nullptr;
    }

    u32 lba1 = IO::inb(disk->m_io_base + ATA_REG_LBA1);
    u32 lba2 = IO::inb(disk->m_io_base + ATA_REG_LBA2);

    if (lba1 != 0 && lba2 != 0) {
        return nullptr;
    }

    while (true) {
        status = IO::inb(disk->m_control_base);

        if ((status & ATA_SR_ERR) == 0 || (status & ATA_SR_DRQ) == 0) {
            break;
        }
    }

    if ((status & ATA_SR_ERR) != 0) {
        return nullptr;
    }

    u8 identity_buffer[512];
    u16* identity_buffer_ptr = (u16*)identity_buffer;
    for (u32 i = 0; i < 256; i++) {
        *(identity_buffer_ptr++) = (u32)IO::inw(disk->m_io_base);
    }

    for (u32 i = 0; i < ATA_IDENT_MODEL_LENGTH / 2; i += 2) {
        disk->m_model_number[i] = identity_buffer[ATA_IDENT_MODEL + i + 1];
        disk->m_model_number[i + 1] = identity_buffer[ATA_IDENT_MODEL + i];
    }

    for (i32 i = ATA_IDENT_MODEL_LENGTH - 1; i >= 0; i--) {
        if (disk->m_model_number[i] != ' ' && disk->m_model_number[i] != '\0') {
            break;
        }
        disk->m_model_number[i] = '\0';
    }

    ATAIdentity* ata_info = reinterpret_cast<ATAIdentity*>(identity_buffer);
    disk->m_addressable_blocks = ata_info->user_addressable_sectors;
    dbgprintf("PATADisk", "Disk '%s' created with %d blocks\n", disk->m_model_number, disk->m_addressable_blocks);

    return disk;
}

PATADisk::PATADisk(Bus::PCI::Address address, Channel channel, Type type)
    : m_interrupt_number(type == Master ? ISR_DISK_MASTER : ISR_DISK_SLAVE)
    , m_io_base(channel == Primary ? 0x1F0 : 0x170)
    , m_control_base(channel == Primary ? 0x3f6 : 0x376)
    , m_channel(channel)
    , m_type(type)
    , m_pci_address(address)
{
}

Result PATADisk::read_sector(u8* buffer, u32 lba) const
{
    dbgprintf("PATADisk", "reading\n");
    ScopedSpinlock scoped_lock(s_lock);

    initiate_command(ATA_CMD_READ_PIO, lba, 1);

    s_waiting_status = WaitingStatus(&ProcessManager::current_process());
    s_waiting_status.set_waiting();

    IDT::register_interrupt_handler(m_interrupt_number, disk_interrupts_handler);
    ProcessManager::current_process().set_waiting(s_waiting_status);

    ProcessManager::the().enter_critical();
    u16* ptr = (u16*)buffer;
    for (u16 i = 0; i < 256; ++i) {
        ptr[i] = IO::inw(m_io_base);
    }
    ProcessManager::the().exit_critical();

    IDT::unregister_interrupt_handler(m_interrupt_number);
    return Result(Result::OK);
}

Result PATADisk::write_sector(const u8* buffer, u32 lba) const
{
    ScopedSpinlock scoped_lock(s_lock);

    initiate_command(ATA_CMD_WRITE_PIO, lba, 1);
    s_waiting_status = WaitingStatus(&ProcessManager::current_process());
    s_waiting_status.set_waiting();

    IDT::register_interrupt_handler(m_interrupt_number, disk_interrupts_handler);
    ProcessManager::current_process().set_waiting(s_waiting_status);
    dbgprintf("PATADisk", "writing\n");

    ProcessManager::the().enter_critical();
    for (u16 i = 0; i < 256; i++) {
        IO::outw(m_io_base + ATA_REG_DATA, buffer[i * 2] + (buffer[i * 2 + 1] << 8));
    }

    dbgprintf("PATADisk", "writing critical\n");
    IDT::unregister_interrupt_handler(m_interrupt_number);
    ProcessManager::the().exit_critical();

    return Result(Result::OK);
}

Result PATADisk::initiate_command(u8 command, u32 lba, u8 sectors) const
{
    ProcessManager::the().enter_critical();

    wait_until_ready();

    IO::outb(m_io_base + ATA_REG_HDDEVSEL, 0xE0 | ((m_type == Slave) << 4) | ((lba >> 24) & 0x0F));

    IO::delay(20);

    IO::outb(m_io_base + ATA_REG_SECCOUNT0, sectors);

    IO::outb(m_io_base + ATA_REG_LBA0, lba & 0xFF);
    IO::outb(m_io_base + ATA_REG_LBA1, (lba >> 8) & 0xFF);
    IO::outb(m_io_base + ATA_REG_LBA2, (lba >> 16) & 0xFF);

    wait_until_ready();

    ProcessManager::the().exit_critical();

    IO::outb(m_io_base + ATA_REG_COMMAND, command);
    return Result(Result::OK);
}

void PATADisk::wait_until_ready() const
{
    u8 status = IO::inb(m_control_base);
    while ((status & ATA_SR_BSY) != 0 || (status & ATA_SR_DRDY) == 0) {
        status = IO::inb(m_control_base);
    }
}

void PATADisk::disk_interrupts_handler()
{
    dbgprintf("PATADisk", "Received an interrupt while waiting on %s\n", PATADisk::s_waiting_status.process().name());
    PATADisk::s_waiting_status.set_ready();
}
