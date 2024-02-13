/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_ZONE_H_
#define _MEMORY_ZONE_H_

#include <limits.h>
#include <memory/types.h>
#include <stdint.h>

#define KERNEL_ZONE_LENGTH (4 * MB)

typedef enum physical_zone_type {
    ZONE_KERNEL,
    ZONE_USER,
} physical_zone_type_e;

typedef struct physical_zone {
    uint32_t base_address;
    uint32_t length;
    uint8_t* bitmap;
    uint32_t bitmap_length;
    uint32_t frames_left;
    uint32_t last_allocated_frame_index;
} physical_zone_t;

physical_address_t zone_allocate_frame(physical_zone_t*, const physical_address_t address, const uint32_t count);

physical_address_t zone_allocate_frame_first(physical_zone_t*);

uint32_t zone_free_frame(physical_zone_t*, const physical_address_t address);

void zone_dump(physical_zone_t*, physical_zone_type_e);

#endif
