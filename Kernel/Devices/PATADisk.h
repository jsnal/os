/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/Devices/ATA.h>

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

    static PATADisk* create(Channel, Type);

    Bus::PCI::Address pci_address() const { return m_pci_address; }

private:
    PATADisk(Bus::PCI::Address, Channel, Type);

    static void disk_interrupts_handler();

    char m_model_number[ATA_IDENT_MODEL_LENGTH];
    u32 m_addressable_blocks { 0 };
    u16 m_io_base { 0 };
    u16 m_control_base { 0 };
    Channel m_channel;
    Type m_type;
    Bus::PCI::Address m_pci_address;
};
