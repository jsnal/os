#include <Kernel/Logger.h>
#include <Kernel/Memory/Zone.h>

using namespace Memory;

Result Zone::allocate_frame(const PhysicalAddress address, PhysicalAddress* allocations, u32 number_of_pages)
{
    if (address < m_lower_address || address > m_upper_address) {
        return Types::AddressOutOfRange;
    }

    if (!address.is_page_aligned()) {
        return Types::NotPageAligned;
    }

    ResultOr<PhysicalAddress> allocated_address;
    u32 last_allocated_frame_index = m_last_allocated_frame_index;
    u32 pages_allocated = 0;

    m_last_allocated_frame_index = (address.get() - m_lower_address.get()) / Types::PageSize;

    for (u32 i = 0; i < number_of_pages; i++) {
        allocated_address = allocate_frame();

        if (allocated_address.is_error()) {
            m_last_allocated_frame_index = last_allocated_frame_index;
            return allocated_address.error();
        }

        if (allocations != nullptr) {
            *(allocations + i) = allocated_address.value();
        }
        pages_allocated++;
    }

    m_last_allocated_frame_index = last_allocated_frame_index;

    if (pages_allocated < number_of_pages - 1) {
        return Types::OutOfMemory;
    }

    dbgprintf("Zone", "Allocated physical pages from 0x%x to 0x%x\n", address, address + number_of_pages * Types::PageSize);
    return Result::OK;
}

ResultOr<PhysicalAddress> Zone::allocate_frame()
{
    PhysicalAddress address;

    for (u32 i = m_last_allocated_frame_index; i < m_pages_in_range; i++) {
        if (!m_bitmap.get(i)) {
            m_bitmap.set(i, true);
            address = m_lower_address.offset(Types::PageSize * i);
            m_last_allocated_frame_index = i;
            return address;
        }

        if (i >= m_pages_in_range) {
            i = 0;
        } else if (i == m_last_allocated_frame_index - 1) {
            break;
        }
    }

    dbgprintf("Zone", "No free pages left!\n");
    return Result(Types::OutOfMemory);
}

Result Zone::free_frame(const PhysicalAddress address)
{
    if (address < m_lower_address || address > m_upper_address) {
        return Types::AddressOutOfRange;
    }

    if (!address.is_page_aligned()) {
        return Types::NotPageAligned;
    }

    u32 address_index = (address - m_lower_address) / Types::PageSize;
    m_bitmap.set(address_index, false);

    dbgprintf("Zone", "Freed physical page at 0x%x\n", address);
    return Result::OK;
}
