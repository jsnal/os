/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/PCI.h>
#include <Kernel/IO.h>

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

namespace Bus::PCI {

static u32 get_io_address(Address& address, u8 field)
{
    return (address.bus << 16) | (address.slot << 11) | (address.function << 8) | (field & 0xFC) | ((u32)0x80000000);
}

u8 read8(Address address, u8 field)
{
    IO::outl(PCI_ADDRESS_PORT, get_io_address(address, field));
    return IO::inb(PCI_DATA_PORT + (field & 3));
}

u16 read16(Address address, u8 field)
{
    IO::outl(PCI_ADDRESS_PORT, get_io_address(address, field));
    return IO::inw(PCI_DATA_PORT + (field & 2));
}

u32 read32(Address address, u8 field)
{

    IO::outl(PCI_ADDRESS_PORT, get_io_address(address, field));
    return IO::inl(PCI_DATA_PORT);
}

}
