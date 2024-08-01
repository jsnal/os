/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Logger.h>
#include <Kernel/Memory/PhysicalRegion.h>
#include <Kernel/Memory/Types.h>
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
    m_total_pages = (m_upper - m_lower) / Types::PageSize;
    u8* bitmap_address = static_cast<u8*>(kcalloc(m_total_pages / 8));
    m_bitmap = Bitmap::wrap(bitmap_address, m_total_pages);

    return m_total_pages;
}

ResultReturn<PhysicalAddress> PhysicalRegion::allocate_page()
{
    PhysicalAddress address;

    if (m_used_pages == m_total_pages) {
        return Result(Types::OutOfMemory);
    }

    for (u32 i = m_last_allocated_page; i < m_total_pages; i++) {
        if (!m_bitmap.get(i)) {
            m_bitmap.set(i, true);
            m_used_pages++;
            m_last_allocated_page = i;
            dbgprintf_if(DEBUG_PHYSICAL_REGION, "PhysicalRegion", "Allocated physical page\n");
            return m_lower.offset(Types::PageSize * i);
        }

        if (i >= m_total_pages) {
            i = 0;
        } else if (i == m_last_allocated_page - 1) {
            break;
        }
    }

    dbgprintf("PhysicalRegion", "No free pages left!\n");
    return Result(Types::OutOfMemory);
}

Result PhysicalRegion::free_page(PhysicalAddress address)
{
    if (address < m_lower || address > m_upper || m_used_pages == 0) {
        return Types::AddressOutOfRange;
    }

    if (!address.is_page_aligned()) {
        return Types::NotPageAligned;
    }

    u32 address_index = (address - m_lower) / Types::PageSize;
    m_bitmap.set(address_index, false);
    m_used_pages--;

    dbgprintf_if(DEBUG_PHYSICAL_REGION, "PhysicalRegion", "Freed physical page at 0x%x\n", address);
    return Result::OK;
}
