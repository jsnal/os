/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/Devices/ATA.h>
#include <Kernel/Process/Spinlock.h>
#include <Kernel/Process/WaitingStatus.h>
#include <Universal/Result.h>

class PATADisk {
public:
    enum Channel {
        Primary,
        Secondary
    };

    enum Type {
        Master,
        Slave
    };

    PATADisk(Bus::PCI::Address, Channel, Type);

    static UniquePtr<PATADisk> create(Channel, Type);

    Result read_sector(u8* buffer, u32 lba) const;

    Result write_sector(const u8* buffer, u32 lba) const;

    Bus::PCI::Address pci_address() const { return m_pci_address; }

private:
    static void disk_interrupts_handler();

    Result initiate_command(u8 command, u32 lba, u8 sectors) const;

    void wait_until_ready() const;

    static Spinlock s_lock;
    static WaitingStatus s_waiting_status;

    char m_model_number[ATA_IDENT_MODEL_LENGTH];
    u8 m_interrupt_number { 0 };
    u32 m_addressable_blocks { 0 };
    u16 m_io_base { 0 };
    u16 m_control_base { 0 };
    Channel m_channel;
    Type m_type;
    Bus::PCI::Address m_pci_address;
};
