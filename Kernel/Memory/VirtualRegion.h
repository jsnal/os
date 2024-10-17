/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/AddressAllocator.h>
#include <Kernel/Memory/Paging.h>
#include <Universal/Array.h>
#include <Universal/LinkedList.h>
#include <Universal/Number.h>
#include <Universal/UniquePtr.h>

class VirtualRegion : public LinkedListNode<VirtualRegion> {
public:
    VirtualRegion(const AddressRange&, u8 access);
    ~VirtualRegion();

    static UniquePtr<VirtualRegion> create_kernel_region(const AddressRange& address_range, u8 access);
    static UniquePtr<VirtualRegion> create_kernel_region_at(PhysicalAddress, const AddressRange& address_range, u8 access);

    static UniquePtr<VirtualRegion> create_user_region(const AddressRange& address_range, u8 access);

    enum Access {
        Read = 1,
        Write = 2,
        Execute = 4,
    };

    const AddressRange& address_range() const { return m_address_range; }

    void map(PageDirectory&);
    Result unmap(PageDirectory&);
    Result free();
    Result contains(VirtualAddress);

    inline size_t page_count() { return ceiling_divide(m_address_range.length(), Types::PageSize); }

    inline bool is_readable() { return m_access & Read; }
    inline bool is_writable() { return m_access & Write; }
    inline bool is_executable() { return m_access & Execute; }

    VirtualAddress lower() const { return m_address_range.lower(); }
    VirtualAddress upper() const { return m_address_range.upper(); }

    VirtualRegion* m_next { nullptr };
    VirtualRegion* m_previous { nullptr };

private:
    void invalidate_page(VirtualAddress);

    AddressRange m_address_range;

    Array<PhysicalAddress> m_physical_pages;
    SharedPtr<PageDirectory> m_page_directory;

    u8 m_access { Read };
    bool m_is_kernel_region { false };
};
