/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_TYPES_H_
#define _MEMORY_TYPES_H_

#include <limits.h>
#include <stdint.h>

#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_IMAGE_VIRTUAL_BASE 0xC0100000

#define IS_PAGE_ALIGNED(address) (!(address & 0xFFF))
#define PAGE_ALIGN(address) ((address & 0xFFFFF000) + PAGE_SIZE)
#define PAGE_ROUND_UP(address) ((address + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER 0x4

#define MEM_ERR_CHECK(v) (!(IS_PAGE_ALIGNED(v)))
#define MEM_ERR_NONE 0
#define MEM_ERR_GENERAL 1
#define MEM_ERR_OUT_OF_MEMORY 2
#define MEM_ERR_ADDRESS_OUT_OF_RANGE 3
#define MEM_ERR_NOT_PAGE_ALIGNED 4
#define MEM_ERR_PAGE_ALREADY_PRESENT 5

typedef uint32_t physical_address_t;
typedef uint32_t virtual_address_t;

static inline virtual_address_t physical_to_virtual(physical_address_t address)
{
    return address + KERNEL_VIRTUAL_BASE;
}

static inline physical_address_t virtual_to_physical(virtual_address_t address)
{
    return address - KERNEL_VIRTUAL_BASE;
}

#endif
