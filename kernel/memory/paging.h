/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_PAGING_H_
#define _MEMORY_PAGING_H_

#include <limits.h>
#include <memory/types.h>
#include <multiboot.h>
#include <stdint.h>

#define PAGE_DIRECTORY_INDEX(virtual_address) (((virtual_address_t)virtual_address) >> 22)
#define PAGE_TABLE_INDEX(virtual_address) ((((virtual_address_t)virtual_address) & 0x3fffff) >> 12)
#define PAGE_FRAME_INDEX(virtual_address) (((virtual_address_t)virtual_address) & (0xfff))
#define PAGE_ENTRY_ADDR(page_entry) (page_entry & 0xfffff000)

#define PAGE_DIRECTORY_LENGTH 1024
#define PAGE_TABLE_LENGTH 1024

// Basic virtual memory map:
//   [0xC0100000][kernel][PMM bitmap][heap][free pages][0xC0400000]
typedef struct paging_kernel_information {
    virtual_address_t* page_directory;
    virtual_address_t* page_table;
    virtual_address_t bitmap;
    virtual_address_t heap;
    virtual_address_t free_pages;
} paging_kernel_information_t;

void paging_flush_tlb();

void paging_invalidate_page(const virtual_address_t);

uint32_t paging_map_kernel_page(virtual_address_t* page_directory, virtual_address_t virtual_address, physical_address_t physical_address);

void init_paging(virtual_address_t* kernel_page_directory);

#endif
