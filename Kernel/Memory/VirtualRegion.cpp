/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/VirtualRegion.h>

VirtualRegion::VirtualRegion(const AddressRange& address_range, u8 access, bool is_kernel_region)
    : m_address_range(address_range)
    , m_physical_pages(page_count())
    , m_access(access)
    , m_is_kernel_region(is_kernel_region)
{
    MM.add_virtual_region(*this);
}

VirtualRegion::~VirtualRegion()
{
    MM.remove_virtual_region(*this);
}

UniquePtr<VirtualRegion> VirtualRegion::create_kernel_region(const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access, true);
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = MM.allocate_physical_kernel_page();
    }
    return region;
}

UniquePtr<VirtualRegion> VirtualRegion::create_kernel_dma_region(const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access, true);
    auto start_address = MM.allocate_physical_contiguous_kernel_pages(region->m_physical_pages.size());
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = start_address.offset(i * Memory::kPageSize);
    }
    return region;
}

UniquePtr<VirtualRegion> VirtualRegion::create_kernel_region_at(PhysicalAddress physical_address, const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access, true);
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = physical_address.offset(i * Memory::kPageSize);
    }
    return region;
}

UniquePtr<VirtualRegion> VirtualRegion::create_user_region(const AddressRange& address_range, u8 access)
{
    auto region = make_unique_ptr<VirtualRegion>(address_range, access, false);
    for (size_t i = 0; i < region->m_physical_pages.size(); i++) {
        region->m_physical_pages[i] = MM.allocate_physical_user_page();
    }
    return region;
}

void VirtualRegion::map(PageDirectory& page_directory)
{
    if (m_page_directory.is_null()) {
        m_page_directory = page_directory;
    }

    for (size_t i = 0; i < m_physical_pages.size(); i++) {
        auto page_virtual_address = m_address_range.lower().offset(i * Memory::kPageSize);
        auto& page_table_entry = MM.get_page_table_entry(page_directory, page_virtual_address, !m_is_kernel_region);
        auto physical_page = m_physical_pages[i];

        page_table_entry.set_physical_page_base(physical_page.get());
        page_table_entry.set_user(!m_is_kernel_region);
        page_table_entry.set_present(is_readable());
        page_table_entry.set_read_write(is_writable());

        Memory::invalidate_page(page_virtual_address);
    }
}

Result VirtualRegion::unmap(PageDirectory& page_directory)
{
    if (m_page_directory.is_null() || m_page_directory != page_directory) {
        return Status::Failure;
    }

    for (size_t i = 0; i < m_physical_pages.size(); i++) {
        auto page_virtual_address = m_address_range.lower().offset(i * Memory::kPageSize);
        auto& page_table_entry = MM.get_page_table_entry(page_directory, page_virtual_address, !m_is_kernel_region);
        auto physical_page = m_physical_pages[i];

        page_table_entry.set_physical_page_base(0);
        page_table_entry.set_user(false);
        page_table_entry.set_present(false);
        page_table_entry.set_read_write(false);

        Memory::invalidate_page(page_virtual_address);
    }

    return Status::OK;
}

Result VirtualRegion::free()
{
    for (size_t i = 0; i < m_physical_pages.size(); i++) {
        if (m_is_kernel_region) {
            MM.free_physical_kernel_page(m_physical_pages[i]);
        } else {
            MM.free_physical_user_page(m_physical_pages[i]);
        }
    }
    return Status::OK;
}

bool VirtualRegion::contains(VirtualAddress address)
{
    return address >= m_address_range.lower() && address < m_address_range.upper();
}

bool VirtualRegion::is_accessible(VirtualAddress address, size_t length)
{
    return contains(address) && contains(address + length);
}

Result VirtualRegion::copy(const VirtualRegion& dest)
{
    for (size_t i = 0; i < m_physical_pages.size(); i++) {
        auto* p = ENSURE_TAKE(MM.temporary_map(dest.physical_pages()[i])).ptr();
        memcpy(p, lower().ptr() + Memory::kPageSize * i, Memory::kPageSize);
        MM.temporary_unmap();
    }

    return Status::OK;
}
