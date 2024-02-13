/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_PMM_H_
#define _MEMORY_PMM_H_

#include <limits.h>
#include <memory/memory.h>
#include <memory/types.h>
#include <memory/zone.h>
#include <multiboot.h>
#include <stdint.h>

extern uint32_t kernel_end;

typedef struct physical_region {
    uint32_t base_address;
    uint32_t frames_left;
    uint32_t last_allocated_frame_index;
    uint32_t length;
    uint8_t* bitmap;
} physical_region_t;

void init_pmm(memory_kernel_information_t*, const multiboot_information_t* multiboot);

physical_address_t pmm_allocate_frame(physical_zone_type_e zone_type, const physical_address_t address, const uint32_t count);

physical_address_t pmm_allocate_frame_first(physical_zone_type_e);

uint32_t pmm_free_frame(physical_zone_type_e, const physical_address_t);

#endif
