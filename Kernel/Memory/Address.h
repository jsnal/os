/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

#define PAGE_DIRECTORY_INDEX(virtual_address) ((((u32)virtual_address.get()) >> 22) & 0x3ff);
#define PAGE_TABLE_INDEX(virtual_address) ((((u32)virtual_address.get()) >> 12) & 0x3ff)

class VirtualAddress {
public:
    VirtualAddress()
    {
    }

    VirtualAddress(u32 address)
        : m_address(address)
    {
    }

    operator int() const { return m_address; }

    bool is_page_aligned() const { return ((m_address & 0xfff) == 0); }

    u32 get() const { return m_address; }
    void set(u32 address) { m_address = address; }
    VirtualAddress offset(u32 offset) const { return VirtualAddress(m_address + offset); }

    bool is_null() const { return m_address == 0; }

    u8* ptr() { return reinterpret_cast<u8*>(m_address); }
    const u8* ptr() const { return reinterpret_cast<const u8*>(m_address); }

    u32 page_base() const { return m_address & 0xfffff000; }

    bool operator>(const VirtualAddress& o) const { return m_address > o.get(); }
    bool operator<(const VirtualAddress& o) const { return m_address < o.get(); }
    bool operator>=(const VirtualAddress& o) const { return m_address >= o.get(); }
    bool operator<=(const VirtualAddress& o) const { return m_address <= o.get(); }

private:
    u32 m_address { 0 };
};

class PhysicalAddress {
public:
    PhysicalAddress()
    {
    }

    PhysicalAddress(u32 address)
        : m_address(address)
    {
    }

    operator int() const { return m_address; }

    bool is_page_aligned() const { return ((m_address & 0xfff) == 0); }

    u32 get() const { return m_address; }
    void set(u32 address) { m_address = address; }
    void add(u32 a) { m_address += a; }
    PhysicalAddress offset(u32 offset) const { return PhysicalAddress(m_address + offset); }

    u8* ptr() { return reinterpret_cast<u8*>(m_address); }
    const u8* ptr() const { return reinterpret_cast<const u8*>(m_address); }

    u32 page_base() const { return m_address & 0xfffff000; }

    bool operator>(const PhysicalAddress& o) const { return m_address > o.get(); }
    bool operator<(const PhysicalAddress& o) const { return m_address < o.get(); }
    bool operator>=(const PhysicalAddress& o) const { return m_address >= o.get(); }
    bool operator<=(const PhysicalAddress& o) const { return m_address <= o.get(); }

private:
    u32 m_address { 0 };
};

inline u32 operator-(const PhysicalAddress& a, const PhysicalAddress& b)
{
    return a.get() - b.get();
}
