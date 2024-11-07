/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/PCI.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Universal/UniquePtr.h>

class E1000NetworkCard {
public:
    static UniquePtr<E1000NetworkCard> detect();

    E1000NetworkCard(Bus::PCI::Address, u8 interrupt_line);

private:
    void detect_eeprom();
    u32 read_from_eeprom(u8);

    void read_mac_address();

    void out8(u16 address, u8 value);
    void out16(u16 address, u16 value);
    void out32(u16 address, u32 value);
    u8 in8(u16 address);
    u16 in16(u16 address);
    u32 in32(u16 address);

    Bus::PCI::Address m_pci_address;
    u8 m_interrupt_line { 0 };
    PhysicalAddress m_mmio_physical_base;
    UniquePtr<VirtualRegion> m_mmio_region;
    bool m_eeprom_exists { false };

    UniquePtr<VirtualRegion> m_tx_region;
    // TODO: Make MAC address class
    u8 m_mac_address[6];
};
