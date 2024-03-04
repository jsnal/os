/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Function.h>
#include <Universal/Types.h>

namespace Bus::PCI {

#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_IDE_CONTROLLER 0x0101

struct Address {
    u8 bus;
    u8 slot;
    u8 function;
};

struct ID {
    u16 vendor;
    u16 device;
};

typedef Function<void(Address, ID, u16)> EnumerateCallback;

u8 read8(Address, u8 field);
u16 read16(Address, u8 field);
u32 read32(Address, u8 field);

void write8();
void write16();
void write32();

void enumerate_devices(EnumerateCallback);

}
