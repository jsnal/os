/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/AddressAllocator.h>

AddressAllocator::AddressAllocator(VirtualAddress base, size_t length)
{
    m_ranges.add_last(AddressRange(base, length));
}

ResultReturn<AddressRange> AddressAllocator::allocate(size_t length)
{
    if (length == 0) {
        return Result(Result::Failure);
    }

    // TODO: Always round up to the nearest page since we always allocate
    //       that much anyways. Maybe don't do this in the future
    length = Types::page_round_up(length);

    int i;
    for (i = 0; i < m_ranges.size(); i++) {
        if (length <= m_ranges[i].length()) {
            break;
        }
    }

    if (i >= m_ranges.size()) {
        return Result(Result::Failure);
    }

    AddressRange address_range_found = m_ranges[i];
    m_ranges.remove(i);

    if (address_range_found.length() == length) {
        return address_range_found;
    }

    m_ranges.add(i, AddressRange(address_range_found.lower().offset(length), address_range_found.length() - length));

#if DEBUG_ADDRESS_ALLOCATOR
    dump();
#endif
    return AddressRange(address_range_found.lower(), length);
}

ResultReturn<AddressRange> AddressAllocator::allocate_at(VirtualAddress address, size_t length)
{
    if (length == 0) {
        return Result(Result::Failure);
    }

    int i;
    for (i = 0; i < m_ranges.size(); i++) {
        if (m_ranges[i].lower() <= address && address <= m_ranges[i].upper()) {
            break;
        }
    }

    if (i >= m_ranges.size()) {
        return Result(Result::Failure);
    }

    AddressRange address_range_found = m_ranges[i];

    if (address_range_found.upper() - address < length) {
        return Result(Result::Failure);
    }

    m_ranges.remove(i);

    size_t length_before_address = address - address_range_found.lower();
    size_t length_after_address = address_range_found.upper() - address.offset(length);
    if (length_after_address > 0 && length_before_address == 0) {
        m_ranges.add(i, AddressRange(address_range_found.lower().offset(length), length_after_address));
    } else if (length_after_address == 0 && length_before_address > 0) {
        m_ranges.add(i, AddressRange(address_range_found.lower(), length_before_address));
    } else if (length_after_address > 0 && length_before_address > 0) {
        m_ranges.add(i, AddressRange(address_range_found.lower().offset(length_before_address + length), length_after_address));
        m_ranges.add(i, AddressRange(address_range_found.lower(), length_before_address));
    }

#if DEBUG_ADDRESS_ALLOCATOR
    dump();
#endif
    return AddressRange(address, length);
}

Result AddressAllocator::free(AddressRange address_range)
{
    for (int i = 0; i < m_ranges.size(); i++) {
        if (m_ranges[i].upper() == address_range.lower()) {
            m_ranges[i].add_length(address_range.length());
            goto merge;
        }
    }
    m_ranges.add_last(address_range);

merge:
    m_ranges.sort([&](auto& a, auto& b) {
        return a.lower() > b.lower();
    });

    ArrayList<AddressRange> merged_ranges(m_ranges.size());

    for (int i = 0; i < m_ranges.size(); i++) {
        if (merged_ranges.empty()) {
            merged_ranges.add_last(m_ranges[i]);
            continue;
        }

        if (m_ranges[i].lower() == merged_ranges.last().upper()) {
            merged_ranges.last().add_length(m_ranges[i].length());
            continue;
        }

        merged_ranges.add_last(m_ranges[i]);
    }

    m_ranges = move(merged_ranges);

#if DEBUG_ADDRESS_ALLOCATOR
    dump();
#endif
    return Result::OK;
}

#if DEBUG_ADDRESS_ALLOCATOR
void AddressAllocator::dump()
{
    dbgprintf("AddressAllocator", "Dumping address ranges:\n");
    for (int i = 0; i < m_ranges.size(); i++) {
        dbgprintf("AddressAllocator", "  %u: 0x%x - 0x%x\n", i, m_ranges[i].lower(), m_ranges[i].upper());
    }
}
#endif
