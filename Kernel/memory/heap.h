/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_HEAP_H_
#define _MEMORY_HEAP_H_

#include <memory/types.h>

#define HEAP_SIZE (2 * MB)

void init_heap(virtual_address_t heap_start);

#endif
