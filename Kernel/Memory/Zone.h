/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Logger.h>
#include <Kernel/Memory/Address.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/kmalloc.h>
#include <Universal/Bitmap.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

#define KERNEL_REGION_LENGTH (4 * MB)

template<typename AddressType>
class Region {
public:
    Region()
        : m_lower_address(AddressType())
        , m_upper_address(AddressType())
        , m_bitmap(Bitmap::create())
    {
    }

    Region(u32 base_address, u32 length)
        : m_lower_address(AddressType(base_address))
        , m_upper_address(AddressType(base_address + length))
        , m_pages_in_range(length / Memory::Types::PageSize)
    {
        size_t bitmap_size_in_bits = length / Memory::Types::PageSize;
        u8* bitmap_address = static_cast<u8*>(kcalloc(bitmap_size_in_bits / 8));
        m_bitmap = Bitmap::wrap(bitmap_address, bitmap_size_in_bits);
        dbgprintf("Region", "%d KiB Zone created from %x to %x\n", (m_upper_address - m_lower_address) / 1024, m_lower_address, m_upper_address);
    }

    [[nodiscard]] Result allocate_frame(const AddressType address, AddressType* allocations, u32 number_of_pages)
    {
        if (address < m_lower_address || address > m_upper_address) {
            return Types::AddressOutOfRange;
        }

        if (!address.is_page_aligned()) {
            return Types::NotPageAligned;
        }

        ResultOr<AddressType> allocated_address;
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

    [[nodiscard]] inline Result allocate_frame(u32 address, AddressType* allocations, u32 number_of_pages)
    {
        return allocate_frame(AddressType(address), allocations, number_of_pages);
    }

    [[nodiscard]] ResultOr<AddressType> allocate_frame()
    {
        AddressType address;

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

        dbgprintf("Region", "No free pages left!\n");
        return Result(Types::OutOfMemory);
    }

    [[nodiscard]] Result free_frame(const AddressType address)
    {
        if (address < m_lower_address || address > m_upper_address) {
            return Types::AddressOutOfRange;
        }

        if (!address.is_page_aligned()) {
            return Types::NotPageAligned;
        }

        u32 address_index = (address - m_lower_address) / Types::PageSize;
        m_bitmap.set(address_index, false);

        dbgprintf("Region", "Freed physical page at 0x%x\n", address);
        return Result::OK;
    }

    const AddressType lower_address() const { return m_lower_address; }

    const AddressType upper_address() const { return m_upper_address; }

    Bitmap bitmap() const { return m_bitmap; }

private:
    AddressType m_lower_address;
    AddressType m_upper_address;
    u32 m_pages_in_range { 0 };
    u32 m_last_allocated_frame_index { 0 };
    Bitmap m_bitmap;
};
