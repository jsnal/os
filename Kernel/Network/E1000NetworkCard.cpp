#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Network/E1000NetworkCard.h>

#define REG_CTRL 0x0000
#define REG_STATUS 0x0008
#define REG_EEPROM 0x0014
#define REG_CTRL_EXT 0x0018
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

#define TSTA_DD (1 << 0) // Descriptor Done
#define TSTA_EC (1 << 1) // Excess Collisions
#define TSTA_LC (1 << 2) // Late Collision
#define LSTA_TU (1 << 3) // Transmit Underrun

#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8

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
    dbgprintf("E1000NetworkCard", "E1000 network card found at 0x%x\n", network_card_address);
    return make_unique_ptr<E1000NetworkCard>(network_card_address, interrupt_line);
}

E1000NetworkCard::E1000NetworkCard(Bus::PCI::Address address, u8 interrupt_line)
    : m_pci_address(address)
    , m_interrupt_line(interrupt_line)
{
    m_mmio_physical_base = Bus::PCI::read_BAR0(m_pci_address);
    size_t mmio_size = Bus::PCI::get_BAR_size(m_pci_address, Bus::PCI::Bar::Zero);

    m_mmio_region = MM.allocate_kernel_region_at(m_mmio_physical_base, mmio_size);

    detect_eeprom();
    dbgprintf("E1000NetworkCard", "EEPROM exists: %d\n", m_eeprom_exists);

    read_mac_address();
    dbgprintf("E1000NetworkCard", "MAC Address: %x:%x:%x:%x:%x:%x\n", m_mac_address[0], m_mac_address[1], m_mac_address[2],
        m_mac_address[3], m_mac_address[4], m_mac_address[5]);

    rx_init();
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
    if (m_eeprom_exists) {
        uint32_t temp;
        temp = read_from_eeprom(0);
        m_mac_address[0] = temp & 0xff;
        m_mac_address[1] = temp >> 8;
        temp = read_from_eeprom(1);
        m_mac_address[2] = temp & 0xff;
        m_mac_address[3] = temp >> 8;
        temp = read_from_eeprom(2);
        m_mac_address[4] = temp & 0xff;
        m_mac_address[5] = temp >> 8;
    } else {
        uint8_t* mem_base_mac_8 = (uint8_t*)(m_mmio_region->lower().offset(0x5400).get());
        uint32_t* mem_base_mac_32 = (uint32_t*)(m_mmio_region->lower().offset(0x5400).get());
        if (mem_base_mac_32[0] != 0) {
            for (int i = 0; i < 6; i++) {
                m_mac_address[i] = mem_base_mac_8[i];
            }
        }
    }
}

void E1000NetworkCard::rx_init()
{
    m_rx_desc_region = MM.allocate_kernel_dma_region(sizeof(struct rx_desc) * E1000_NUM_RX_DESC);
    m_rx_buffer_region = MM.allocate_kernel_dma_region(8192 * E1000_NUM_RX_DESC);
    uint8_t* ptr = const_cast<u8*>(m_rx_desc_region->physical_pages()[0].ptr());
    struct rx_desc* descs = (struct rx_desc*)ptr;

    //    for (u16 i = 0; i < E1000_NUM_RX_DESC; i++) {
    //        rx_descs[i] = (struct e1000_rx_desc*)((uint8_t*)descs + i * 16);
    //        rx_descs[i]->addr = (uint64_t)(uint8_t*)(kmalloc_ptr->khmalloc(8192 + 16));
    //        rx_descs[i]->status = 0;
    //    }

    //    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32));
    //    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

    //    writeCommand(REG_RXDESCLO, (uint64_t)ptr);
    //    writeCommand(REG_RXDESCHI, 0);

    //    writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);

    //    writeCommand(REG_RXDESCHEAD, 0);
    //    writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
    //    rx_cur = 0;
    //    writeCommand(REG_RCTRL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}

void E1000NetworkCard::tx_init()
{
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