/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Function.h>
#include <Universal/Types.h>

namespace Bus::PCI {

enum Type {
    Bridge = 0x0604,
    MassStorage = 0x0101,
    Display = 0x0300,
};

enum VendorId {
    VirtIO = 0x1af4,
    Intel = 0x8086,
    LegacyEmulator = 0x1234,
    VirtualBox = 0x80ee
};

struct Address {
    u8 bus;
    u8 slot;
    u8 function;
};

struct ID {
    u16 vendor;
    u16 device;
};

union Command {
    struct [[gnu::packed]] Attributes {
        bool io_space : 1;
        bool memory_space : 1;
        bool bus_master : 1;
        bool special_cycles : 1;
        bool memory_write_invalidate_enable : 1;
        bool vga_palette_snoop : 1;
        bool parity_error_response : 1;
        bool : 1;
        bool serr_enable : 1;
        bool fast_back_to_back_enable : 1;
        bool interrupt_disable : 1;
        u8 reserved2 : 5;
    } attributes;
    u16 value;
};

typedef Function<void(Address, ID, u16)> EnumerateCallback;

u8 read8(Address, u8 field);
u16 read16(Address, u8 field);
u32 read32(Address, u8 field);

void write8(Address, u8 field, u8 value);
void write16(Address, u8 field, u16 value);
void write32(Address, u8 field, u32 value);

void enable_interrupt(Address);

void enumerate_devices(EnumerateCallback);

}
