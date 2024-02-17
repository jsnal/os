#include <Logger.h>
#include <Memory/Zone.h>

#define DEBUG_TAG "Zone"

PhysicalAddress Zone::allocate_frame(const PhysicalAddress address, u32 number_of_pages)
{
    if (address < m_lower_address || address > m_upper_address) {
        return PhysicalAddress(PhysicalAddress::AddressOutOfRange);
    }

    if (!address.is_page_aligned()) {
        return PhysicalAddress(PhysicalAddress::NotPageAligned);
    }

    u32 address_index = 0;

    for (u32 i = 0; i < number_of_pages; i++) {
        address_index = ((address.get() + (PAGE_SIZE * i)) - m_lower_address.get()) / PAGE_SIZE;

        if (m_bitmap.get(address_index)) {
            return PhysicalAddress(PhysicalAddress::AlreadyPresent);
        }

        m_bitmap.set(address_index, true);
    }

    dbgprintf("Allocated %d physical pages starting at 0x%x\n", number_of_pages, address);
    return address;
}

PhysicalAddress Zone::allocate_frame()
{
    PhysicalAddress address;

    for (u32 i = m_last_allocated_frame_index; i < m_pages_in_range; i++) {
        if (!m_bitmap.get(i)) {
            m_bitmap.set(i, true);
            address = m_lower_address.offset(PAGE_SIZE * i);
            m_last_allocated_frame_index = i;
            dbgprintf("Allocated phyiscal page at 0x%x\n", address);
            return address;
        }

        if (i >= m_pages_in_range) {
            i = 0;
        } else if (i == m_last_allocated_frame_index - 1) {
            break;
        }
    }

    dbgprintf("No free pages left!\n");
    return PhysicalAddress(PhysicalAddress::OutOfMemory);
}

PhysicalAddress Zone::free_frame(const PhysicalAddress address)
{
    if (address < m_lower_address || address > m_upper_address) {
        return PhysicalAddress(PhysicalAddress::AddressOutOfRange);
    }

    if (!address.is_page_aligned()) {
        return PhysicalAddress(PhysicalAddress::NotPageAligned);
    }

    u32 address_index = (address - m_lower_address) / PAGE_SIZE;
    m_bitmap.set(address_index, false);

    dbgprintf("Freed physical page at 0x%x\n", address);
    return PhysicalAddress(0);
}
