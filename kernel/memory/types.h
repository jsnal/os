/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_TYPES_H_
#define _MEMORY_TYPES_H_

#include <limits.h>
#include <stdint.h>

#define PAGE_ALIGN(address) ((address & 0xFFFFF000) + PAGE_SIZE)

typedef uint32_t physical_address_t;
typedef uint32_t virtual_address_t;

#endif
