/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/VirtualRegion.h>

VirtualRegion::VirtualRegion(const AddressRange& address_range, u8 access)
    : m_address_range(address_range)
    , m_physical_pages(page_count())
    , m_access(access)
    , m_is_kernel_region(true)
{
    MemoryManager::the().add_virtual_region(*this);
}

VirtualRegion::~VirtualRegion()
{
    MemoryManager::the().remove_virtual_region(*this);
}

UniquePtr<VirtualRegion> VirtualRegion::create_kernel_region(const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access);
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = MemoryManager::the().allocate_physical_kernel_page();
    }
    return region;
}

UniquePtr<VirtualRegion> VirtualRegion::create_kernel_region_at(PhysicalAddress physical_address, const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access);
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = physical_address.offset(i * Types::PageSize);
    }
    return region;
}

UniquePtr<VirtualRegion> VirtualRegion::create_user_region(const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access);
    region->m_is_kernel_region = false;
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = MemoryManager::the().allocate_physical_user_page();
    }
    return region;
}

void VirtualRegion::free_user_region(const VirtualRegion& region)
{
    for (size_t i = 0; i < region.m_physical_pages.size(); i++) {
    }
}

void VirtualRegion::map(PageDirectory& page_directory)
{
    if (m_page_directory.is_null()) {
        m_page_directory = page_directory;
    }

    for (size_t i = 0; i < m_physical_pages.size(); i++) {
        auto page_virtual_address = m_address_range.lower().offset(i * Types::PageSize);
        auto& page_table_entry = MemoryManager::the().get_page_table_entry(page_directory, page_virtual_address, !m_is_kernel_region);
        auto physical_page = m_physical_pages[i];

        page_table_entry.set_physical_page_base(physical_page.get());
        page_table_entry.set_user(!m_is_kernel_region);
        page_table_entry.set_present(is_readable());
        page_table_entry.set_read_write(is_writable());

        invalidate_page(page_virtual_address);
    }
}

void VirtualRegion::invalidate_page(VirtualAddress address)
{
    asm volatile("invlpg [%0]"
                 :
                 : "r"(address.get()));
}
