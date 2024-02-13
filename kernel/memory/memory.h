/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_MEMORY_H_
#define _MEMORY_MEMORY_H_

#include <memory/types.h>
#include <multiboot.h>

typedef struct memory_kernel_information {
    virtual_address_t* page_directory;
    virtual_address_t* page_table;
    virtual_address_t bitmap;
    virtual_address_t heap;
    virtual_address_t free_pages;
} memory_kernel_information_t;

void init_memory(physical_address_t* boot_page_directory, const multiboot_information_t*);

#endif
