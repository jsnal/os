/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Universal/ArrayList.h>
#include <Universal/Logger.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

#define DEBUG_ADDRESS_ALLOCATOR 0

class AddressRange {
public:
    AddressRange() { }

    AddressRange(VirtualAddress base, size_t length)
        : m_base(base)
        , m_length(length)
    {
    }

    size_t length() const { return m_length; }
    void add_length(size_t length) { m_length += length; }

    const VirtualAddress upper() const { return VirtualAddress(m_base + m_length); }
    const VirtualAddress lower() const { return m_base; }

    bool operator==(const AddressRange other) const { return this->m_length == other.m_length && this->m_base == other.m_base; }

private:
    VirtualAddress m_base;
    size_t m_length;
};

class AddressAllocator {
public:
    AddressAllocator(VirtualAddress base, size_t length);

    ResultReturn<AddressRange> allocate(size_t);

    ResultReturn<AddressRange> allocate_at(VirtualAddress, size_t);

    Result free(AddressRange);

private:
#if DEBUG_ADDRESS_ALLOCATOR
    void dump();
#endif

    ArrayList<AddressRange> m_ranges;
};
