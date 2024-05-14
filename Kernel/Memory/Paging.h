/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Kernel/Memory/AddressAllocator.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

class PageTableEntry {
public:
    enum Flags {
        Present = 1 << 0,
        ReadWrite = 1 << 1,
        UserSupervisor = 1 << 2,
    };

    PageTableEntry() { }

    PageTableEntry(u32 address)
        : m_address(address)
    {
    }

    void* physical_page_base() { return reinterpret_cast<void*>(m_address.page_base()); }
    void set_physical_page_base(u32 address)
    {
        m_address = m_address & 0xfff;
        m_address = m_address | (address & 0xfffff000);
    }

    const VirtualAddress address() const { return m_address; }
    VirtualAddress address() { return m_address; }

    bool is_present() const { return m_address & Present; }
    void set_present(bool set) { set_bit(Present, set); }

    bool is_read_write() const { return m_address & ReadWrite; }
    void set_read_write(bool set) { set_bit(ReadWrite, set); }

    bool is_user_supervisor() const { return m_address & UserSupervisor; }
    void set_user_supervisor(bool set) { set_bit(UserSupervisor, set); }

private:
    void set_bit(u32 bit, bool value)
    {
        if (value) {
            m_address.set(m_address | bit);
        } else {
            m_address.set(m_address & ~bit);
        }
    }

    VirtualAddress m_address;
};

class PageDirectoryEntry {
public:
    enum Flags {
        Present = 1 << 0,
        ReadWrite = 1 << 1,
        UserSupervisor = 1 << 2,
    };

    PageDirectoryEntry() { }

    PageDirectoryEntry(u32 address)
        : m_address(address)
    {
    }

    PageTableEntry* page_table_base() { return reinterpret_cast<PageTableEntry*>(m_address.get() & 0xfffff000); }
    void set_page_table_base(u32 address)
    {
        m_address = m_address & 0xfff;
        m_address = m_address | (address & 0xfffff000);
    }

    const VirtualAddress address() const { return m_address; }
    VirtualAddress address() { return m_address; }

    bool is_present() const { return m_address & Present; }
    void set_present(bool set) { set_bit(Present, set); }

    bool is_read_write() const { return m_address & ReadWrite; }
    void set_read_write(bool set) { set_bit(ReadWrite, set); }

    bool is_user_supervisor() const { return m_address & UserSupervisor; }
    void set_user_supervisor(bool set) { set_bit(UserSupervisor, set); }

private:
    void set_bit(u32 bit, bool value)
    {
        if (value) {
            m_address = m_address | bit;
        } else {
            m_address = m_address & ~bit;
        }
    }

    VirtualAddress m_address;
};

class PageDirectory : public ShareCounted<PageDirectory> {
public:
    static SharedPtr<PageDirectory> create_kernel_page_directory(PhysicalAddress address)
    {
        return adopt(*new PageDirectory(address));
    }

    PageDirectory()
        : m_directory_page_base()
        , m_address_allocator(0, 0)
    {
    }

    PhysicalAddress base() const { return m_directory_page_base; }

    PageDirectoryEntry* entries() { return reinterpret_cast<PageDirectoryEntry*>(m_directory_page_base.get()); }

    AddressAllocator& address_allocator() { return m_address_allocator; }

private:
    PageDirectory(PhysicalAddress address)
        : m_directory_page_base(address)
        , m_address_allocator(KERNEL_VIRTUAL_BASE + KERNEL_IMAGE_LENGTH, 0x3F000000)
    {
    }

    PhysicalAddress m_directory_page_base;

    AddressAllocator m_address_allocator;
};
