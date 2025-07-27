/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Universal/Bitmap.h>
#include <Universal/Expected.h>
#include <Universal/RefCounted.h>
#include <Universal/SharedPtr.h>

class PhysicalRegion : public RefCounted<PhysicalRegion> {
public:
    static SharedPtr<PhysicalRegion> create(PhysicalAddress lower, PhysicalAddress upper);

    PhysicalRegion() { }

    PhysicalRegion(PhysicalRegion&&) { }

    void expand(PhysicalAddress lower, PhysicalAddress upper);

    [[nodiscard]] bool includes(PhysicalAddress address) const
    {
        return address > m_lower && address < m_upper;
    }

    u32 commit();
    Expected<PhysicalAddress> allocate_contiguous_pages(u32 number_of_pages);
    Expected<PhysicalAddress> allocate_page();
    Result free_page(PhysicalAddress);

    const PhysicalAddress lower() const { return m_lower; }
    const PhysicalAddress upper() const { return m_upper; }

private:
    PhysicalRegion(PhysicalAddress lower, PhysicalAddress upper);

    void allocate_page_at(u32 page_index);
    Expected<u32> find_contiguous_pages(u32 number_of_pages);

    PhysicalAddress m_lower;
    PhysicalAddress m_upper;
    u32 m_total_pages { 0 };
    u32 m_used_pages { 0 };
    u32 m_last_allocated_page { 0 };
    Bitmap m_bitmap;
};
