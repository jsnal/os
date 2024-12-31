/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Boot/multiboot.h>
#include <Kernel/Memory/Paging.h>
#include <Kernel/Memory/PhysicalRegion.h>
#include <Kernel/Memory/VMObject.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Universal/ArrayList.h>
#include <Universal/Result.h>
#include <Universal/Types.h>
#include <Universal/UniquePtr.h>

#define MM MemoryManager::the()

class MemoryManager final {
public:
    static MemoryManager& the();

    static void init(u32* boot_page_directory, const multiboot_information_t*);

    MemoryManager();

    PageDirectory& kernel_page_directory() { return *m_kernel_page_directory; }
    const PageDirectory& kernel_page_directory() const { return *m_kernel_page_directory; }

    PhysicalAddress allocate_physical_kernel_page();
    PhysicalAddress allocate_physical_contiguous_kernel_pages(u32);
    Result free_physical_kernel_page(PhysicalAddress);
    PhysicalAddress allocate_physical_user_page();
    Result free_physical_user_page(PhysicalAddress);

    UniquePtr<VirtualRegion> allocate_kernel_region(size_t size);
    UniquePtr<VirtualRegion> allocate_kernel_dma_region(size_t size);
    UniquePtr<VirtualRegion> allocate_kernel_region_at(PhysicalAddress physical_address, size_t size);
    void free_kernel_region(VirtualRegion&);

    void protected_map(PageDirectory&, VirtualAddress, size_t);
    void identity_map(PageDirectory&, VirtualAddress, size_t);

    VirtualAddress temporary_map(PhysicalAddress);
    void temporary_unmap();

    void copy_kernel_page_directory(PageDirectory&);

    PageTableEntry& get_page_table_entry(PageDirectory&, VirtualAddress, bool is_kernel);
    Result remove_page_table_entry(PageDirectory& page_directory, VirtualAddress virtual_address);

    void add_vm_object(VMObject&);
    void remove_vm_object(VMObject&);

    void add_virtual_region(VirtualRegion&);
    void remove_virtual_region(VirtualRegion&);

private:
    void internal_init(u32* boot_page_directory, const multiboot_information_t*);

    void flush_tlb();

    SharedPtr<PageDirectory> m_kernel_page_directory;

    ArrayList<SharedPtr<PhysicalRegion>> m_kernel_physical_regions;
    ArrayList<SharedPtr<PhysicalRegion>> m_user_physical_regions;

    LinkedList<VirtualRegion> m_kernel_virtual_regions;
    LinkedList<VirtualRegion> m_user_virtual_regions;

    LinkedList<VMObject> m_vm_objects;
};
