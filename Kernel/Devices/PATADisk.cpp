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

PATADisk* PATADisk::create(Channel channel, Type type)
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
    return new PATADisk(ide_controller_address, channel, type);
}

PATADisk::PATADisk(Bus::PCI::Address address, Channel channel, Type type)
    : m_io_base(channel == Primary ? 0x1F0 : 0x170)
    , m_control_base(channel == Primary ? 0x3f6 : 0x376)
    , m_channel(channel)
    , m_type(type)
    , m_pci_address(address)
{
    IDT::register_interrupt_handler(ISR_DISK, disk_interrupts_handler);

    IO::outb(m_io_base + ATA_REG_HDDEVSEL, 0xA0 | (type == Slave ? 0x10 : 0x00));
    IO::outb(m_io_base + ATA_REG_SECCOUNT0, 0);
    IO::outb(m_io_base + ATA_REG_LBA0, 0);
    IO::outb(m_io_base + ATA_REG_LBA1, 0);
    IO::outb(m_io_base + ATA_REG_LBA2, 0);

    IO::outb(m_io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    u32 status = 0;
    while ((status = IO::inb(m_control_base)) & ATA_SR_BSY)
        ;

    if (status == 0) {
        return;
    }

    u32 lba1 = IO::inb(m_io_base + ATA_REG_LBA1);
    u32 lba2 = IO::inb(m_io_base + ATA_REG_LBA2);

    if (lba1 != 0 && lba2 != 0) {
        return;
    }

    u32 error = 0;
    u32 drq = 0;
    while (true) {
        error = IO::inb(m_control_base) & ATA_SR_ERR;
        drq = IO::inb(m_control_base) & ATA_SR_DRQ;

        if (error == 0 || drq == 0) {
            break;
        }
    }

    if (error != 0) {
        return;
    }

    u8 identity_buffer[512];
    u16* identity_buffer_ptr = (u16*)identity_buffer;
    for (u32 i = 0; i < 256; i++) {
        *(identity_buffer_ptr++) = (u32)IO::inw(m_io_base);
    }

    for (u32 i = 0; i < ATA_IDENT_MODEL_LENGTH / 2; i += 2) {
        m_model_number[i] = identity_buffer[ATA_IDENT_MODEL + i + 1];
        m_model_number[i + 1] = identity_buffer[ATA_IDENT_MODEL + i];
    }

    for (i32 i = ATA_IDENT_MODEL_LENGTH - 1; i >= 0; i--) {
        if (m_model_number[i] != ' ' && m_model_number[i] != '\0') {
            break;
        }
        m_model_number[i] = '\0';
    }

    ATAIdentity* ata_info = reinterpret_cast<ATAIdentity*>(identity_buffer);
    m_addressable_blocks = ata_info->user_addressable_sectors;
    dbgprintf("PATADisk", "Disk '%s' created with %d blocks\n", m_model_number, m_addressable_blocks);
}

Result PATADisk::read_sector(u8* buffer, u32 lba) const
{
    // Select device, use LBA addressing mode
    IO::outb(m_io_base + ATA_REG_HDDEVSEL, 0xE0 | ((m_type == Slave) << 4) | ((lba >> 24) & 0x0F));
    IO::delay(20);

    // Send sector count
    IO::outb(m_io_base + ATA_REG_SECCOUNT0, 1);

    // Send LBA address
    IO::outb(m_io_base + ATA_REG_LBA0, lba & 0xFF);
    IO::outb(m_io_base + ATA_REG_LBA1, (lba >> 8) & 0xFF);
    IO::outb(m_io_base + ATA_REG_LBA2, (lba >> 16) & 0xFF);

    // Send read command
    IO::outb(m_io_base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    // Read data from data port
    u16* ptr = (u16*)buffer;
    for (u16 i = 0; i < 256; ++i) {
        ptr[i] = IO::inw(m_io_base);
    }

    return Result(Result::OK);
}

void PATADisk::disk_interrupts_handler()
{
    dbgprintf("PATADisk", "Received an interrupts\n");
}
