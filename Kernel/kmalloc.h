/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Universal/Types.h>

#define NEW_FOREVER \
public:             \
    void* operator new(size_t size) { return kmalloc_forever(size); }

void kmalloc_init();

void* kmalloc_forever(size_t);
