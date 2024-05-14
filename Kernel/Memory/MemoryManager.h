/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Boot/multiboot.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/Memory/Paging.h>
#include <Kernel/Memory/PhysicalRegion.h>
#include <Kernel/Memory/VMObject.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Universal/ArrayList.h>
#include <Universal/Result.h>
#include <Universal/Types.h>
#include <Universal/UniquePtr.h>

class MemoryManager final {
public:
    static MemoryManager& the();

    static void init(u32* boot_page_directory, const multiboot_information_t*);

    MemoryManager();

    const PageDirectory& kernel_page_directory() const { return *m_kernel_page_directory; }

    const PageTableEntry& kernel_page_table() const { return *m_kernel_page_table; }

    const PMM& pmm() const { return *m_pmm; }

    PhysicalAddress allocate_physical_kernel_page();

    UniquePtr<VirtualRegion> allocate_kernel_region(size_t size);
    UniquePtr<VirtualRegion> allocate_kernel_region_at(PhysicalAddress physical_address, size_t size);

    void identity_map(PageDirectory&, VirtualAddress, size_t);

    PageTableEntry& get_page_table_entry(PageDirectory&, VirtualAddress, bool is_kernel);

    ResultOr<VirtualAddress> map_kernel_region(PhysicalAddress, size_t);

    Result unmap_kernel_region(PhysicalAddress, size_t);

    [[nodiscard]] Result map_kernel_page(VirtualAddress, PhysicalAddress);

    [[nodiscard]] Result unmap_kernel_page(VirtualAddress);

    void add_vm_object(VMObject&);
    void remove_vm_object(VMObject&);

    void add_virtual_region(VirtualRegion&);
    void remove_virtual_region(VirtualRegion&);

private:
    void internal_init(u32* boot_page_directory, const multiboot_information_t*);

    void flush_tlb();

    void invalidate_page(VirtualAddress);

    SharedPtr<PageDirectory> m_kernel_page_directory;

    PageTableEntry* m_kernel_page_table;

    ArrayList<SharedPtr<PhysicalRegion>> m_kernel_physical_regions;
    ArrayList<SharedPtr<PhysicalRegion>> m_user_physical_regions;

    LinkedList<VirtualRegion> m_kernel_virtual_regions;
    LinkedList<VirtualRegion> m_user_virtual_regions;

    LinkedList<VMObject> m_vm_objects;

    const PMM* m_pmm;

    Region<VirtualAddress> m_kernel_virtual_region;
};
