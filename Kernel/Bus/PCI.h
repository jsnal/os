/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

namespace Bus::PCI {

struct Address {
    u8 bus;
    u8 slot;
    u8 function;
};

u8 read8(Address, u8 field);
u16 read16(Address, u8 field);
u32 read32(Address, u8 field);

void write8();
void write16();
void write32();

}
