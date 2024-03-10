/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/Devices/ATA.h>
#include <Kernel/Process/Spinlock.h>
#include <Kernel/Process/WaitingStatus.h>
#include <Universal/Result.h>

#define SECTOR_SIZE 512

class PATADisk final : public IRQHandler {
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

    Result read_sectors(u8* buffer, u32 lba, u32 sectors);
    Result read_block(u8* buffer, u32 lba);

    Result write_sectors(const u8* buffer, u32 lba, u32 sectors);
    Result write_block(const u8* buffer, u32 lba);

    void clear_interrupts() const;

    Bus::PCI::Address pci_address() const { return m_pci_address; }

private:
    void handle() override;

    static void disk_interrupts_handler();

    void initiate_command(u8 command, u32 lba, u8 sectors);

    void wait_until_ready() const;

    static Spinlock s_lock;
    static WaitingStatus s_waiting_status;
    static bool s_currently_waiting;

    char m_model_number[ATA_IDENT_MODEL_LENGTH];
    u32 m_addressable_blocks { 0 };
    u16 m_io_base { 0 };
    u16 m_control_base { 0 };
    Channel m_channel;
    Type m_type;
    Bus::PCI::Address m_pci_address;
};
