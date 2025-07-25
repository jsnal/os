/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IDT.h>
#include <Kernel/Devices/ATA.h>
#include <Kernel/Devices/PATADisk.h>
#include <Kernel/IO.h>
#include <Kernel/Process/ProcessManager.h>
#include <Universal/Logger.h>

#define DEBUG_PATA_DISK 0

UniquePtr<PATADisk> PATADisk::create(Channel channel, Type type)
{
    Bus::PCI::Address ide_controller_address = {};
    Bus::PCI::ID ide_controller_id = {};

    Bus::PCI::enumerate_devices([&](Bus::PCI::Address address, Bus::PCI::ID id, u16 type) {
        if (type == Bus::PCI::Type::MassStorage) {
            ide_controller_address = address;
            ide_controller_id = id;
            return;
        }
    });

    if (ide_controller_address.bus == 0 && ide_controller_address.slot == 0 && ide_controller_address.function == 0) {
        return nullptr;
    }

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
    dbgprintln("PATADisk", "%s Disk '%s' created with %d blocks", channel == Primary ? "Primary" : "Secondary", disk->m_model_number, disk->m_addressable_blocks);
    dbgprintln_if(DEBUG_PATA_DISK, "PATADisk", "Disk at PCI id %x:%x", ide_controller_id.vendor, ide_controller_id.device);

    // NOTE: The IDENTIFY command generates an interrupt that needs to be cleared before
    //       issuing any other commands to the controller.
    disk->clear_interrupts();
    return disk;
}

PATADisk::PATADisk(Bus::PCI::Address address, Channel channel, Type type)
    : IRQHandler(channel == Primary ? IRQ::DISK_PRIMARY : IRQ::DISK_SECONDARY)
    , BlockDevice()
    , m_io_base(channel == Primary ? 0x1F0 : 0x170)
    , m_control_base(channel == Primary ? 0x3F6 : 0x376)
    , m_channel(channel)
    , m_type(type)
    , m_pci_address(address)
{
}

Result PATADisk::read_blocks(u32 block, u32 count, u8* buffer)
{
    dbgprintln_if(DEBUG_PATA_DISK, "PATADisk", "Reading %u sectors into 0x%x @ block %u", count, buffer, block);

    ScopedSpinlock scoped_lock(m_lock);

    initiate_command(ATA_CMD_READ_PIO, block, count);

    for (u32 i = 0; i < count; i++) {
        PM.enter_critical();
        wait_until_ready();

        u16* ptr = (u16*)buffer;
        for (u16 j = 0; j < 256; j++) {
            ptr[j] = IO::inw(m_io_base);
        }
        buffer += SECTOR_SIZE;

        PM.exit_critical();
    }

    disable_irq();

    return Result(Status::OK);
}

Result PATADisk::write_blocks(u32 block, u32 count, const u8* buffer)
{
    dbgprintln_if(DEBUG_PATA_DISK, "PATADisk", "Writing %u sectors from 0x%x @ block %u", count, buffer, block);

    ScopedSpinlock scoped_lock(m_lock);

    initiate_command(ATA_CMD_WRITE_PIO, block, count);

    for (u32 i = 0; i < count; i++) {
        PM.enter_critical();
        wait_until_ready();

        for (u16 j = 0; j < 256; j++) {
            IO::outw(m_io_base + ATA_REG_DATA, buffer[j * 2] + (buffer[j * 2 + 1] << 8));
        }
        buffer += SECTOR_SIZE;

        PM.exit_critical();
    }

    disable_irq();

    IO::outb(m_io_base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    wait_until_ready();

    return Result(Status::OK);
}

void PATADisk::clear_interrupts() const
{
    u8 status = IO::inb(m_io_base + ATA_REG_STATUS);
    dbgprintln_if(DEBUG_PATA_DISK, "PATADisk", "Cleared status register: DRQ=%d BSY=%d DRDY=%d ERR=%d", (status & ATA_SR_DRQ) == 0, (status & ATA_SR_BSY) == 0, (status & ATA_SR_DRDY) == 0, (status & ATA_SR_ERR) == 0);
}

void PATADisk::initiate_command(u8 command, u32 lba, u8 sectors)
{
    PM.enter_critical();
    wait_until_ready();

    IO::outb(m_io_base + ATA_REG_HDDEVSEL, 0xE0 | (m_type == Slave ? 0x8 : 0x0) | (lba & 0xF000000) >> 24);

    IO::delay(20);

    IO::outb(m_io_base + ATA_REG_SECCOUNT0, sectors);

    IO::outb(m_io_base + ATA_REG_LBA0, lba & 0xFF);
    IO::outb(m_io_base + ATA_REG_LBA1, (lba >> 8) & 0xFF);
    IO::outb(m_io_base + ATA_REG_LBA2, (lba >> 16) & 0xFF);

    wait_until_ready();

    enable_irq();
    PM.exit_critical();
    IO::outb(m_io_base + ATA_REG_COMMAND, command);

    dbgprintln_if(DEBUG_PATA_DISK, "PATADisk", "Done initiating command");
}

void PATADisk::wait_until_ready() const
{
    u8 status = IO::inb(m_control_base);
    while ((status & ATA_SR_BSY) || !(status & ATA_SR_DRDY)) {
        status = IO::inb(m_control_base);
    }
}

void PATADisk::handle_irq(const InterruptRegisters&)
{
    dbgprintln_if(DEBUG_PATA_DISK, "PATADisk", "Received an interrupt while waiting");
    clear_interrupts();
}
