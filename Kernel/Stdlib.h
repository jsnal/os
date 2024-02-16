/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Types.h>

extern "C" {

size_t strlen(const char* str);

void* memset(void* s, int c, size_t n);

void* memcpy(void* dest, const void* src, size_t n);

void* memmove(void* dest, const void* src, size_t n);
}
