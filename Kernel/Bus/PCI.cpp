/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/PCI.h>
#include <Kernel/IO.h>

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

#define PCI_VENDOR_ID 0x0
#define PCI_DEVICE_ID 0x2
#define PCI_COMMAND 0x4
#define PCI_SUBCLASS 0xA
#define PCI_CLASS 0xB
#define PCI_HEADER_TYPE 0xE
#define PCI_SECONDARY_BUS 0x19
#define PCI_MULTI_FUNCTION 0x80

#define PCI_NO_VENDOR 0xFFFF

#define PCI_SLOTS_ON_BUS 32
#define PCI_FUNCTIONS_ON_SLOT 8

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24

namespace Bus::PCI {

static void enumerate_functions(u8 bus, u8 slot, u8 functions, EnumerateCallback& callback);
static void enumerate_slot(u8 bus, u8 slot, EnumerateCallback& callback);
static void enumerate_bus(u8 bus, EnumerateCallback& callback);

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

u32 read_BAR0(Address address)
{
    return read32(address, PCI_BAR0);
}

u32 read_BAR1(Address address)
{
    return read32(address, PCI_BAR1);
}

u32 read_BAR2(Address address)
{
    return read32(address, PCI_BAR2);
}

u32 read_BAR3(Address address)
{
    return read32(address, PCI_BAR3);
}

u32 read_BAR4(Address address)
{
    return read32(address, PCI_BAR4);
}

u32 read_BAR5(Address address)
{
    return read32(address, PCI_BAR5);
}

void write8(Address address, u8 field, u8 value)
{
    IO::outl(PCI_ADDRESS_PORT, get_io_address(address, field));
    IO::outb(PCI_DATA_PORT + (field & 3), value);
}

void write16(Address address, u8 field, u16 value)
{
    IO::outl(PCI_ADDRESS_PORT, get_io_address(address, field));
    IO::outw(PCI_DATA_PORT + (field & 2), value);
}

void write32(Address address, u8 field, u16 value)
{
    IO::outl(PCI_ADDRESS_PORT, get_io_address(address, field));
    IO::outw(PCI_DATA_PORT, value);
}

void enable_interrupt(Address address)
{
    Command command = {
        .value = read16(address, PCI_COMMAND)
    };
    command.attributes.interrupt_disable = false;
    write16(address, PCI_COMMAND, command.value);
}

static void enumerate_functions(u8 bus, u8 slot, u8 functions, EnumerateCallback& callback)
{
    Address functions_address = { bus, slot, functions };

    u16 type = (read8(functions_address, PCI_CLASS) << 8) + read8(functions_address, PCI_SUBCLASS);
    if (type == Type::Bridge) {
        u8 second_bus = read8(functions_address, PCI_SECONDARY_BUS);
        enumerate_bus(second_bus, callback);
    }

    u16 vendor = read16(functions_address, PCI_VENDOR_ID);
    u16 device = read16(functions_address, PCI_DEVICE_ID);
    callback(functions_address, { vendor, device }, type);
}

static void enumerate_slot(u8 bus, u8 slot, EnumerateCallback& callback)
{
    Address slot_common_header_address = { bus, slot, 0 };

    if (read16(slot_common_header_address, PCI_VENDOR_ID) == PCI_NO_VENDOR) {
        return;
    }

    enumerate_functions(bus, slot, 0, callback);

    if (read8(slot_common_header_address, PCI_HEADER_TYPE) == 1) {
        return;
    }

    Address functions_address;
    for (u8 function = 1; function < PCI_FUNCTIONS_ON_SLOT; function++) {
        functions_address = { bus, slot, function };
        if (read16(functions_address, PCI_VENDOR_ID) != PCI_NO_VENDOR) {
            enumerate_functions(bus, slot, function, callback);
        }
    }
}

static void enumerate_bus(u8 bus, EnumerateCallback& callback)
{
    for (u8 slot = 0; slot < PCI_SLOTS_ON_BUS; slot++) {
        enumerate_slot(bus, slot, callback);
    }
}

void enumerate_devices(EnumerateCallback callback)
{
    Address host_bridge_header = { 0, 0, 0 };
    if ((read8(host_bridge_header, PCI_HEADER_TYPE) & PCI_MULTI_FUNCTION) == 0) {
        enumerate_bus(0, callback);
    } else {
        for (u8 function = 0; function < 8; function++) {
            Address address = { 0, 0, function };
            if (read16(address, PCI_VENDOR_ID) != PCI_NO_VENDOR) {
                enumerate_bus(function, callback);
            }
        }
    }
}
}
