#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Network/E1000NetworkCard.h>

#define REG_EEPROM 0x0014

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

    m_tx_region = MM.allocate_kernel_dma_region(8196 * 16);

    detect_eeprom();
    dbgprintf("E1000NetworkCard", "EEPROM exists: %d\n", m_eeprom_exists);

    read_mac_address();
    dbgprintf("E1000NetworkCard", "MAC Address: %x:%x:%x:%x:%x:%x\n", m_mac_address[0], m_mac_address[1], m_mac_address[2],
        m_mac_address[3], m_mac_address[4], m_mac_address[5]);
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
