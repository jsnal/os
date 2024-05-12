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

ResultOr<AddressRange> AddressAllocator::allocate(size_t length)
{
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

    size_t length_left_over = address_range_found.length() - length;
    m_ranges.add_last(AddressRange(address_range_found.lower().offset(length_left_over), length_left_over));

    return AddressRange(address_range_found.lower(), length);
}

ResultOr<AddressRange> AddressAllocator::allocate_at(VirtualAddress address, size_t length)
{
    int i;
    for (i = 0; i < m_ranges.size(); i++) {
        if (m_ranges[i].lower() < address && address < m_ranges[i].upper()) {
            break;
        }
    }

    AddressRange address_range_found = m_ranges[i];

    if (i >= m_ranges.size()) {
        return Result(Result::Failure);
    }

    if (address - address_range_found.lower() > length) {
        return Result(Result::Failure);
    }

    m_ranges.remove(i);

    size_t length_before_address = address - address_range_found.lower();
    size_t length_after_address = address_range_found.upper() - address;
    if (length_after_address > 0 && length_before_address == 0) {
        m_ranges.add_last(AddressRange(address_range_found.upper().offset(length_after_address), length_after_address));
    } else if (length_after_address == 0 && length_before_address > 0) {
        m_ranges.add_last(AddressRange(address_range_found.lower().offset(length_before_address), length_before_address));
    } else if (length_after_address > 0 && length_before_address > 0) {
        m_ranges.add_last(AddressRange(address_range_found.upper().offset(length_after_address), length_after_address));
        m_ranges.add_last(AddressRange(address_range_found.lower().offset(length_before_address), length_before_address));
    }

    return AddressRange(address, length);
}

Result AddressAllocator::free(AddressRange address_range)
{
}

void AddressAllocator::sort()
{
    m_ranges.sort([&](AddressRange a, AddressRange b) {
        return a.lower() < b.lower();
    });
}
