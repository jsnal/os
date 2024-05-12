/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Universal/ArrayList.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

class AddressRange {
public:
    AddressRange() { }

    AddressRange(VirtualAddress base, size_t length)
        : m_base(base)
        , m_length(length)
    {
    }

    const size_t length() const { return m_length; }
    void set_length(size_t length) { m_length = length; }

    const VirtualAddress upper() const { return VirtualAddress(m_base + m_length); }
    const VirtualAddress lower() const { return m_base; }

private:
    VirtualAddress m_base;
    size_t m_length;
};

class AddressAllocator {
public:
    AddressAllocator(VirtualAddress base, size_t length);

    ResultOr<AddressRange> allocate(size_t);

    ResultOr<AddressRange> allocate_at(VirtualAddress, size_t);

    Result free(AddressRange);

private:
    void dump();

    ArrayList<AddressRange> m_ranges;
};
