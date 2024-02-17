/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Types.h>
#include <Memory/PMM.h>
#include <Memory/Paging.h>
#include <multiboot.h>

class MemoryManager final {
public:
    static MemoryManager& the();

    static void init(u32* boot_page_directory, const multiboot_information_t*);

    MemoryManager();

    PageDirectoryEntry* kernel_page_directory() const { return m_kernel_page_directory; }

    PageTableEntry* kernel_page_table() const { return m_kernel_page_table; }

    PMM pmm() const { return m_pmm; }

private:
    void internal_init(u32* boot_page_directory, const multiboot_information_t*);

    PageDirectoryEntry* m_kernel_page_directory;

    PageTableEntry* m_kernel_page_table;

    PMM m_pmm;
};
