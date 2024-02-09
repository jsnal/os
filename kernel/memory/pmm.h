/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_PMM_H_
#define _MEMORY_PMM_H_

#include <limits.h>
#include <memory/types.h>
#include <multiboot.h>
#include <stdint.h>

#define SET_BIT(b, i) b = b | (1 << (i % CHAR_BIT))
#define CLEAR_BIT(b, i) b = (b) & (~(1 << (i % CHAR_BIT)))
#define IS_SET(b, i) ((b >> (i % CHAR_BIT)) & 1)

#define PMM_ERROR 0xFFFFFFFF

extern uint32_t kernel_end;

typedef struct physical_region {
    uint32_t base_address;
    uint32_t frames_left;
    uint32_t last_allocated_frame_index;
    uint32_t length;
    uint8_t* bitmap;
} physical_region_t;

void init_pmm(const multiboot_information_t* multiboot);

physical_address_t pmm_allocate_frame(const physical_address_t, const uint32_t count);

physical_address_t pmm_allocate_frame_first();

uint32_t pmm_free_frame(const physical_address_t);

#endif
