/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Kernel/Network/MACAddress.h>
#include <Universal/ByteBuffer.h>
#include <Universal/CircularQueue.h>
#include <Universal/UniquePtr.h>

namespace Network {

class E1000NetworkCard : public IRQHandler {
public:
    static UniquePtr<E1000NetworkCard> detect();

    E1000NetworkCard(Bus::PCI::Address, u8 interrupt_line);

    MACAddress mac_address() const { return m_mac_address; }

    const CircularQueue<ByteBuffer>& rx_queue() const { return m_rx_queue; }
    CircularQueue<ByteBuffer>& rx_queue() { return m_rx_queue; }

    void send(const u8* data, size_t length);

    u32 m_rx_count { 0 };

private:
    struct [[gnu::packed]] rx_desc {
        volatile u64 addr;
        volatile u16 length;
        volatile u16 checksum;
        volatile u8 status;
        volatile u8 errors;
        volatile u16 special;
    };

    struct [[gnu::packed]] tx_desc {
        volatile u64 addr;
        volatile u16 length;
        volatile u8 cso;
        volatile u8 cmd;
        volatile u8 status;
        volatile u8 css;
        volatile u16 special;
    };

    void detect_eeprom();
    u32 read_from_eeprom(u8);

    void read_mac_address();

    rx_desc* rx_descs_base() const { return reinterpret_cast<rx_desc*>(m_rx_desc_region->lower().ptr()); }
    tx_desc* tx_descs_base() const { return reinterpret_cast<tx_desc*>(m_tx_desc_region->lower().ptr()); }

    void link_init();
    void irq_init();
    void rx_init();
    void tx_init();

    void receive();

    void handle_irq(const InterruptRegisters&) override;

    void out8(u16 address, u8 value);
    void out16(u16 address, u16 value);
    void out32(u16 address, u32 value);
    u8 in8(u16 address);
    u16 in16(u16 address);
    u32 in32(u16 address);

    Bus::PCI::Address m_pci_address;
    PhysicalAddress m_mmio_physical_base;
    UniquePtr<VirtualRegion> m_mmio_region;

    u8 m_interrupt_line { 0 };
    bool m_eeprom_exists { false };

    UniquePtr<VirtualRegion> m_tx_desc_region;
    UniquePtr<VirtualRegion> m_tx_buffer_region;
    UniquePtr<VirtualRegion> m_rx_desc_region;
    UniquePtr<VirtualRegion> m_rx_buffer_region;

    CircularQueue<ByteBuffer> m_rx_queue;

    MACAddress m_mac_address;
};

}
