/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Network/E1000NetworkCard.h>
#include <Kernel/Network/Ethernet.h>
#include <Kernel/Process/ProcessManager.h>

#define DEBUG_E1000 (0)

#define REG_CTRL 0x0000
#define REG_STATUS 0x0008
#define REG_EEPROM 0x0014
#define REG_CTRL_EXT 0x0018
#define REG_ICAUSE 0x00C0
#define REG_IMASK 0x00D0
#define REG_RCTRL 0x0100
#define REG_RXDESCLO 0x2800
#define REG_RXDESCHI 0x2804
#define REG_RXDESCLEN 0x2808
#define REG_RXDESCHEAD 0x2810
#define REG_RXDESCTAIL 0x2818

#define REG_TCTRL 0x0400
#define REG_TXDESCLO 0x3800
#define REG_TXDESCHI 0x3804
#define REG_TXDESCLEN 0x3808
#define REG_TXDESCHEAD 0x3810
#define REG_TXDESCTAIL 0x3818

#define REG_RDTR 0x2820   // RX Delay Timer Register
#define REG_RXDCTL 0x2828 // RX Descriptor Control
#define REG_RADV 0x282C   // RX Int. Absolute Delay Timer
#define REG_RSRPD 0x2C00  // RX Small Packet Detect Interrupt

#define REG_TIPG 0x0410 // Transmit Inter Packet Gap
#define ECTRL_SLU 0x40  // set link up

#define RCTL_EN (1 << 1)            // Receiver Enable
#define RCTL_SBP (1 << 2)           // Store Bad Packets
#define RCTL_UPE (1 << 3)           // Unicast Promiscuous Enabled
#define RCTL_MPE (1 << 4)           // Multicast Promiscuous Enabled
#define RCTL_LPE (1 << 5)           // Long Packet Reception Enable
#define RCTL_LBM_NONE (0 << 6)      // No Loopback
#define RCTL_LBM_PHY (3 << 6)       // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF (0 << 8)    // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER (1 << 8) // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH (2 << 8)  // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36 (0 << 12)        // Multicast Offset - bits 47:36
#define RCTL_MO_35 (1 << 12)        // Multicast Offset - bits 46:35
#define RCTL_MO_34 (2 << 12)        // Multicast Offset - bits 45:34
#define RCTL_MO_32 (3 << 12)        // Multicast Offset - bits 43:32
#define RCTL_BAM (1 << 15)          // Broadcast Accept Mode
#define RCTL_VFE (1 << 18)          // VLAN Filter Enable
#define RCTL_CFIEN (1 << 19)        // Canonical Form Indicator Enable
#define RCTL_CFI (1 << 20)          // Canonical Form Indicator Bit Value
#define RCTL_DPF (1 << 22)          // Discard Pause Frames
#define RCTL_PMCF (1 << 23)         // Pass MAC Control Frames
#define RCTL_SECRC (1 << 26)        // Strip Ethernet CRC

// Buffer Sizes
#define RCTL_BSIZE_256 (3 << 16)
#define RCTL_BSIZE_512 (2 << 16)
#define RCTL_BSIZE_1024 (1 << 16)
#define RCTL_BSIZE_2048 (0 << 16)
#define RCTL_BSIZE_4096 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384 ((1 << 16) | (1 << 25))

// Transmit Command
#define CMD_EOP (1 << 0)  // End of Packet
#define CMD_IFCS (1 << 1) // Insert FCS
#define CMD_IC (1 << 2)   // Insert Checksum
#define CMD_RS (1 << 3)   // Report Status
#define CMD_RPS (1 << 4)  // Report Packet Sent
#define CMD_VLE (1 << 6)  // VLAN Packet Enable
#define CMD_IDE (1 << 7)  // Interrupt Delay Enable

// TCTL Register
#define TCTL_EN (1 << 1)      // Transmit Enable
#define TCTL_PSP (1 << 3)     // Pad Short Packets
#define TCTL_CT_SHIFT 4       // Collision Threshold
#define TCTL_COLD_SHIFT 12    // Collision Distance
#define TCTL_SWXOFF (1 << 22) // Software XOFF Transmission
#define TCTL_RTLC (1 << 24)   // Re-transmit on Late Collision

// TSTA Register
#define TSTA_DD (1 << 0) // Descriptor Done
#define TSTA_EC (1 << 1) // Excess Collisions
#define TSTA_LC (1 << 2) // Late Collision
#define LSTA_TU (1 << 3) // Transmit Underrun

// Interrupts
#define INT_TXDW 0x01   // Transmit descriptor written back
#define INT_TXQE 0x02   // Transmit queue empty
#define INT_LSC 0x04    // Link status change
#define INT_RXSEQ 0x08  // Receive Sequence Error
#define INT_RXDMT0 0x10 // Receive Descriptor Minimum Threshold Reached
#define INT_RXO 0x40    // Receiver Overrun
#define INT_RXT0 0x80   // Receiver Timer Interrupt

// Hardcoded sizes
#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8
#define E1000_RX_BUFFER_SIZE 8192
#define E1000_TX_BUFFER_SIZE 8192

namespace Network {

UniquePtr<E1000NetworkCard> E1000NetworkCard::detect()
{
    static const Bus::PCI::ID target_id = { Bus::PCI::VendorId::Intel, Bus::PCI::DeviceId::E1000Emulated };
    Bus::PCI::Address network_card_address = {};

    Bus::PCI::enumerate_devices([&](Bus::PCI::Address const address, Bus::PCI::ID const id, u16 const type) {
        if (id == target_id) {
            network_card_address = address;
            return;
        }
    });

    if (network_card_address.is_null()) {
        return nullptr;
    }

    u8 interrupt_line = Bus::PCI::read_interrupt_line(network_card_address);
    dbgprintf("E1000NetworkCard", "E1000 network card found @ %#08x\n", network_card_address);
    return make_unique_ptr<E1000NetworkCard>(network_card_address, interrupt_line);
}

E1000NetworkCard::E1000NetworkCard(Bus::PCI::Address address, u8 interrupt_line)
    : m_pci_address(address)
    , m_interrupt_line(interrupt_line)
{
    m_mmio_physical_base = Bus::PCI::read_BAR0(m_pci_address);
    size_t mmio_size = Bus::PCI::get_BAR_size(m_pci_address, Bus::PCI::Bar::Zero);
    m_mmio_region = MM.allocate_kernel_region_at(m_mmio_physical_base, mmio_size);

    Bus::PCI::enable_bus_mastering(m_pci_address);

    detect_eeprom();
    read_mac_address();

    link_init();
    irq_init();
    rx_init();
    tx_init();
}

void E1000NetworkCard::detect_eeprom()
{
    out32(REG_EEPROM, 0x1);
    for (int i = 0; i < 1000 && !m_eeprom_exists; i++) {
        u32 val = in32(REG_EEPROM);
        if (val & 0x10) {
            m_eeprom_exists = true;
        }
    }
}

u32 E1000NetworkCard::read_from_eeprom(u8 address)
{
    u16 data = 0;
    u32 tmp = 0;
    if (m_eeprom_exists) {
        out32(REG_EEPROM, (1) | ((uint32_t)(address) << 8));
        while (!((tmp = in32(REG_EEPROM)) & (1 << 4)))
            ;
    } else {
        out32(REG_EEPROM, (1) | ((uint32_t)(address) << 2));
        while (!((tmp = in32(REG_EEPROM)) & (1 << 1)))
            ;
    }
    data = (u16)((tmp >> 16) & 0xFFFF);
    return data;
}

void E1000NetworkCard::read_mac_address()
{
    u8 mac_address[MACAddress::kLength];
    if (m_eeprom_exists) {
        uint32_t tmp = read_from_eeprom(0);
        mac_address[0] = tmp & 0xff;
        mac_address[1] = tmp >> 8;
        tmp = read_from_eeprom(1);
        mac_address[2] = tmp & 0xff;
        mac_address[3] = tmp >> 8;
        tmp = read_from_eeprom(2);
        mac_address[4] = tmp & 0xff;
        mac_address[5] = tmp >> 8;
    } else {
        uint8_t* mem_base_mac_8 = reinterpret_cast<uint8_t*>(m_mmio_region->lower().offset(0x5400).get());
        uint32_t* mem_base_mac_32 = reinterpret_cast<uint32_t*>(m_mmio_region->lower().offset(0x5400).get());
        if (mem_base_mac_32[0] != 0) {
            for (int i = 0; i < MACAddress::kLength; i++) {
                mac_address[i] = mem_base_mac_8[i];
            }
        }
    }

    m_mac_address = MACAddress(mac_address);
    dbgprintf("E1000NetworkCard", "MAC Address: %s\n", m_mac_address.to_string().data());
}

void E1000NetworkCard::link_init()
{
    out32(REG_CTRL, in32(REG_CTRL) | ECTRL_SLU);
    bool state = in32(REG_STATUS) & 0x02;
    dbgprintf_if(DEBUG_E1000, "E1000NetworkCard", "Link state: %s\n", state ? "UP" : "DOWN");
}

void E1000NetworkCard::irq_init()
{
    int irq = Bus::PCI::read8(m_pci_address, PCI_INTERRUPT_LINE);
    set_irq(irq);
    enable_irq();
    out32(REG_IMASK, INT_LSC | INT_RXT0 | INT_RXO);
    in32(REG_ICAUSE);
    Bus::PCI::enable_interrupt(m_pci_address);
}

void E1000NetworkCard::rx_init()
{
    // Allocate all DMA memory for descriptors and buffers ahead of time
    m_rx_desc_region = MM.allocate_kernel_dma_region(sizeof(rx_desc) * E1000_NUM_RX_DESC);
    m_rx_buffer_region = MM.allocate_kernel_dma_region(E1000_RX_BUFFER_SIZE * E1000_NUM_RX_DESC);
    ASSERT((m_rx_desc_region->lower() % 16) == 0);

    u32 physical_desc_start = m_rx_desc_region->physical_pages()[0].get();
    u32 physical_buffer_start = m_rx_buffer_region->physical_pages()[0].get();
    auto descs = rx_descs_base();
    for (u32 i = 0; i < E1000_NUM_RX_DESC; i++) {
        descs[i].addr = physical_buffer_start + (E1000_RX_BUFFER_SIZE * i);
        descs[i].status = 0;
    }

    // Only 32-bit addresses need to be supported so high 4 bytes
    // of descriptor address can always we set to 0
    out32(REG_RXDESCLO, physical_desc_start);
    out32(REG_RXDESCHI, 0);
    out32(REG_RXDESCLEN, E1000_NUM_RX_DESC * sizeof(rx_desc));
    out32(REG_RXDESCHEAD, 0);
    out32(REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
    out32(REG_RCTRL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}

void E1000NetworkCard::tx_init()
{
    // Allocate all DMA memory for descriptors and buffers ahead of time
    m_tx_desc_region = MM.allocate_kernel_dma_region(sizeof(tx_desc) * E1000_NUM_TX_DESC);
    m_tx_buffer_region = MM.allocate_kernel_dma_region(E1000_TX_BUFFER_SIZE * E1000_NUM_TX_DESC);
    ASSERT((m_tx_desc_region->lower() % 16) == 0);

    u32 physical_desc_start = m_tx_desc_region->physical_pages()[0].get();
    u32 physical_buffer_start = m_tx_buffer_region->physical_pages()[0].get();
    auto descs = tx_descs_base();
    for (u32 i = 0; i < E1000_NUM_TX_DESC; i++) {
        descs[i].addr = physical_buffer_start + (E1000_TX_BUFFER_SIZE * i);
        descs[i].cmd = 0;
        descs[i].status = TSTA_DD;
    }

    // Only 32-bit addresses need to be supported so high 4 bytes
    // of descriptor address can always we set to 0
    out32(REG_TXDESCHI, 0);
    out32(REG_TXDESCLO, physical_desc_start);
    out32(REG_TXDESCLEN, E1000_NUM_TX_DESC * sizeof(tx_desc));
    out32(REG_TXDESCHEAD, 0);
    out32(REG_TXDESCTAIL, 0);
    out32(REG_TCTRL, in32(REG_TCTRL) | TCTL_PSP | TCTL_EN);
    out32(REG_TIPG, 0x0060200A);
}

void E1000NetworkCard::send(const u8* data, size_t length)
{
    u32 current_tx_desc = in32(REG_TXDESCTAIL) % E1000_NUM_TX_DESC;
    tx_desc& desc = tx_descs_base()[current_tx_desc];
    u8* buffer = reinterpret_cast<u8*>(m_tx_buffer_region->lower().offset(E1000_TX_BUFFER_SIZE * current_tx_desc).get());

    memcpy(buffer, data, length);
    desc.status = 0;
    desc.length = length;
    desc.cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    out32(REG_TXDESCTAIL, (current_tx_desc + 1) % E1000_NUM_TX_DESC);

    PM.enter_critical();
    while (!(desc.status & 0xff))
        ;
    PM.exit_critical();
    dbgprintln_if(DEBUG_E1000, "E1000NetworkCard", "Sent %u byte frame");
}

void E1000NetworkCard::send(MACAddress destination, const ARPPacket& packet)
{
    constexpr size_t buffer_size = sizeof(EthernetHeader) + sizeof(ARPPacket);
    u8 buffer[buffer_size];

    EthernetHeader& ethernet_header = *reinterpret_cast<EthernetHeader*>(buffer);
    ethernet_header.set_destination(destination);
    ethernet_header.set_source(m_mac_address);
    ethernet_header.set_type(EthernetType::ARP);

    memcpy(buffer + sizeof(EthernetHeader), &packet, sizeof(ARPPacket));
    send(buffer, buffer_size);
}

void E1000NetworkCard::send(MACAddress destination_mac_address, IPv4Address destination_ipv4_address, const u8* payload, size_t payload_size)
{
    size_t buffer_size = sizeof(EthernetHeader) + sizeof(IPv4Packet) + payload_size;
    u8 buffer[buffer_size];

    EthernetHeader& ethernet_header = *reinterpret_cast<EthernetHeader*>(buffer);
    ethernet_header.set_destination(destination_mac_address);
    ethernet_header.set_source(m_mac_address);
    ethernet_header.set_type(EthernetType::IPv4);

    IPv4Packet& ipv4_packet = *reinterpret_cast<IPv4Packet*>(ethernet_header.data());
    ipv4_packet.set_version(4);
    ipv4_packet.set_ihl(5);
    ipv4_packet.set_identification(1);
    ipv4_packet.set_df_bit(true);
    ipv4_packet.set_mf_bit(false);
    ipv4_packet.set_fragment_offset(0);
    ipv4_packet.set_ttl(64);
    ipv4_packet.set_destination(destination_ipv4_address);
    ipv4_packet.set_source(m_ipv4_address);
    ipv4_packet.set_protocol(IPv4Protocol::ICMP);
    ipv4_packet.set_checksum(0);
    ipv4_packet.set_total_length(sizeof(IPv4Packet) + payload_size);

    memcpy(ipv4_packet.data(), payload, payload_size);

    ipv4_packet.set_checksum(calculate_checksum(buffer, buffer_size));

    send(buffer, buffer_size);
}

void E1000NetworkCard::receive()
{
    do {
        u32 current_rx_desc = (in32(REG_RXDESCTAIL) + 1) % E1000_NUM_RX_DESC;
        rx_desc& desc = rx_descs_base()[current_rx_desc];

        if (!(desc.status & 0x1)) {
            break;
        }

        u8* buffer = reinterpret_cast<u8*>(m_rx_buffer_region->lower().offset(E1000_RX_BUFFER_SIZE * current_rx_desc).get());
        dbgprintln_if(DEBUG_E1000, "E1000NetworkCard", "Received %u byte frame", desc.length);

        ByteBuffer queue_buffer(desc.length, true);
        memcpy(queue_buffer.data(), buffer, desc.length);

        ASSERT(!m_rx_queue.is_full());
        m_rx_queue.enqueue(queue_buffer);

        desc.status = 0;
        out32(REG_RXDESCTAIL, current_rx_desc);
    } while (true);
}

void E1000NetworkCard::handle_irq(const InterruptRegisters&)
{
    out32(REG_IMASK, 0x1);

    u32 status = in32(REG_ICAUSE);
    if (status & INT_LSC) {
        link_init();
    }

    if (status & INT_RXO) {
        dbgprintf("E1000NetworkCard", "RX buffer overflowed!\n");
    }

    if (status & INT_RXT0) {
        receive();
    }
}

void E1000NetworkCard::out8(u16 address, u8 value)
{
    *(volatile u8*)(m_mmio_region->lower().offset(address).get()) = value;
}

void E1000NetworkCard::out16(u16 address, u16 value)
{
    *(volatile u16*)(m_mmio_region->lower().offset(address).get()) = value;
}

void E1000NetworkCard::out32(u16 address, u32 value)
{
    *(volatile u32*)(m_mmio_region->lower().offset(address).get()) = value;
}

u8 E1000NetworkCard::in8(u16 address)
{
    return *(volatile u8*)(m_mmio_region->lower().offset(address).get());
}

u16 E1000NetworkCard::in16(u16 address)
{
    return *(volatile u16*)(m_mmio_region->lower().offset(address).get());
}

u32 E1000NetworkCard::in32(u16 address)
{
    return *(volatile u32*)(m_mmio_region->lower().offset(address).get());
}

}
