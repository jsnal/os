/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/PMM.h>
#include <Kernel/Memory/Paging.h>
#include <Kernel/multiboot.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

class MemoryManager final {
public:
    static MemoryManager& the();

    static void init(u32* boot_page_directory, const multiboot_information_t*);

    MemoryManager();

    const PageDirectory& kernel_page_directory() const { return m_kernel_page_directory; }

    const PageTableEntry& kernel_page_table() const { return *m_kernel_page_table; }

    const PMM& pmm() const { return *m_pmm; }

private:
    void internal_init(u32* boot_page_directory, const multiboot_information_t*);

    PageTableEntry& get_page_table_entry(PageDirectory&, VirtualAddress, bool is_kernel);

    [[nodiscard]] Result map_kernel_page(VirtualAddress, PhysicalAddress);

    [[nodiscard]] Result unmap_kernel_page(VirtualAddress);

    void flush_tlb();

    void invalidate_page(VirtualAddress);

    PageDirectory m_kernel_page_directory;

    PageTableEntry* m_kernel_page_table;

    const PMM* m_pmm;
};
