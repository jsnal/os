/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Logger.h>
#include <Kernel/Memory/PagingTypes.h>
#include <Kernel/Memory/PhysicalRegion.h>
#include <Kernel/kmalloc.h>

using namespace Memory;

SharedPtr<PhysicalRegion> PhysicalRegion::create(PhysicalAddress lower, PhysicalAddress upper)
{
    return adopt(*new PhysicalRegion(lower, upper));
}

PhysicalRegion::PhysicalRegion(PhysicalAddress lower, PhysicalAddress upper)
    : m_lower(lower)
    , m_upper(upper)
    , m_bitmap(Bitmap::create())
{
}

void PhysicalRegion::expand(PhysicalAddress lower, PhysicalAddress upper)
{
    if (m_total_pages != 0) {
        return;
    }

    m_lower = lower;
    m_upper = upper;
}

u32 PhysicalRegion::commit()
{
    m_total_pages = (m_upper - m_lower) / Memory::kPageSize;
    u8* bitmap_address = static_cast<u8*>(kcalloc(m_total_pages / 8));
    m_bitmap = Bitmap::wrap(bitmap_address, m_total_pages);

    return m_total_pages;
}

ResultReturn<PhysicalAddress> PhysicalRegion::allocate_contiguous_pages(u32 number_of_pages)
{
    auto contiguous_pages = find_contiguous_pages(number_of_pages);
    if (contiguous_pages.is_error()) {
        return contiguous_pages.error();
    }

    u32 start_page = contiguous_pages.value();
    u32 current_page = start_page;
    for (u32 i = 0; i < number_of_pages; i++) {
        allocate_page_at(current_page++);
    }

    return m_lower.offset(Memory::kPageSize * start_page);
}

ResultReturn<PhysicalAddress> PhysicalRegion::allocate_page()
{
    PhysicalAddress address;

    if (m_used_pages >= m_total_pages) {
        return Result(Memory::kOutOfMemory);
    }

    for (u32 i = m_last_allocated_page; i < m_total_pages; i++) {
        if (!m_bitmap.get(i)) {
            allocate_page_at(i);
            m_last_allocated_page = i;
            return m_lower.offset(Memory::kPageSize * i);
        }

        if (i >= m_total_pages) {
            i = 0;
        } else if (i == m_last_allocated_page - 1) {
            break;
        }
    }

    dbgprintf("PhysicalRegion", "No free pages left!\n");
    return Result(Memory::kOutOfMemory);
}

Result PhysicalRegion::free_page(PhysicalAddress address)
{
    if (address < m_lower || address > m_upper || m_used_pages == 0) {
        return Memory::kAddressOutOfRange;
    }

    if (!address.is_page_aligned()) {
        return Memory::kNotPageAligned;
    }

    u32 address_index = (address - m_lower) / Memory::kPageSize;
    m_bitmap.set(address_index, false);
    m_used_pages--;

    dbgprintf_if(DEBUG_PHYSICAL_REGION, "PhysicalRegion", "Freed physical page at 0x%x\n", address);
    return Result::OK;
}

void PhysicalRegion::allocate_page_at(u32 page_index)
{
    ASSERT(!m_bitmap.get(page_index));
    m_bitmap.set(page_index, true);
    m_used_pages++;
    dbgprintf_if(DEBUG_PHYSICAL_REGION, "PhysicalRegion", "Allocated physical page at 0x%x\n",
        m_lower.offset(Memory::kPageSize * page_index));
}

ResultReturn<u32> PhysicalRegion::find_contiguous_pages(u32 number_of_pages)
{
    if (m_used_pages >= m_total_pages) {
        return Result(Memory::kOutOfMemory);
    }

    u32 start_page = 0;
    u32 number_of_pages_found = 0;
    for (u32 i = 0; i < m_total_pages; i++) {
        if (!m_bitmap.get(i)) {
            number_of_pages_found++;
            if (number_of_pages_found == number_of_pages) {
                break;
            }
        } else {
            number_of_pages_found = 0;
            start_page = i + 1;
        }
    }

    if (number_of_pages_found != number_of_pages) {
        return Result(Result::Failure);
    }
    return start_page;
}
