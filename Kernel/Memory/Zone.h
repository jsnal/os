/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/kmalloc.h>
#include <Universal/Bitmap.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

#define KERNEL_ZONE_LENGTH (4 * MB)

class Zone {
public:
    Zone()
        : m_lower_address(PhysicalAddress())
        , m_upper_address(PhysicalAddress())
        , m_bitmap(Bitmap::create())
    {
    }

    Zone(u32 base_address, u32 length)
        : m_lower_address(PhysicalAddress(base_address))
        , m_upper_address(PhysicalAddress(base_address + length))
        , m_pages_in_range(length / Memory::Types::PageSize)
    {
        size_t bitmap_size_in_bits = length / Memory::Types::PageSize;
        u8* bitmap_address = static_cast<u8*>(kmalloc(bitmap_size_in_bits / 8));
        m_bitmap = Bitmap::wrap(bitmap_address, bitmap_size_in_bits);
        m_bitmap.fill(0);
    }

    [[nodiscard]] Result allocate_frame(const PhysicalAddress address, PhysicalAddress* allocations, u32 number_of_pages);

    [[nodiscard]] inline Result allocate_frame(u32 address, PhysicalAddress* allocations, u32 number_of_pages)
    {
        return allocate_frame(PhysicalAddress(address), allocations, number_of_pages);
    }

    [[nodiscard]] ResultOr<PhysicalAddress> allocate_frame();

    [[nodiscard]] Result free_frame(const PhysicalAddress);

    const PhysicalAddress lower_address() const { return m_lower_address; }

    const PhysicalAddress upper_address() const { return m_upper_address; }

    Bitmap bitmap() const { return m_bitmap; }

private:
    PhysicalAddress m_lower_address;
    PhysicalAddress m_upper_address;
    u32 m_pages_in_range { 0 };
    u32 m_last_allocated_frame_index { 0 };
    Bitmap m_bitmap;
};
