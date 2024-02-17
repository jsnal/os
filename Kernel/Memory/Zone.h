/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Bitmap.h>
#include <LibA/Types.h>
#include <Memory/Address.h>
#include <Memory/Types.h>

#define KERNEL_ZONE_LENGTH (4 * MB)

class Zone {
public:
    explicit Zone()
        : m_lower_address(0)
        , m_upper_address(0)
        , m_bitmap(Bitmap::create())
    {
    }

    Zone(u32 base_address, u32 length, u8* bitmap_address)
        : m_lower_address(base_address)
        , m_upper_address(base_address + length)
        , m_pages_in_range(length / PAGE_SIZE)
        , m_bitmap(Bitmap::wrap(bitmap_address, length / PAGE_SIZE / 8))
    {
        m_bitmap.fill(0);
    }

    PhysicalAddress allocate_frame(const PhysicalAddress, u32 number_of_pages);

    inline PhysicalAddress allocate_frame(u32 address, u32 number_of_pages)
    {
        return allocate_frame(PhysicalAddress(address), number_of_pages);
    }

    PhysicalAddress allocate_frame();

    PhysicalAddress free_frame(const PhysicalAddress);

    PhysicalAddress lower_address() const { return m_lower_address; }

    PhysicalAddress upper_address() const { return m_upper_address; }

    Bitmap bitmap() const { return m_bitmap; }

private:
    PhysicalAddress m_lower_address;
    PhysicalAddress m_upper_address;
    u32 m_pages_in_range;
    u32 m_last_allocated_frame_index { 0 };
    Bitmap m_bitmap;
};
