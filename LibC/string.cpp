/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Stdlib.h>
#include <Universal/Types.h>
#include <limits.h>
#include <string.h>

void* memset(void* dest, int val, size_t length)
{
    return Universal::memset(dest, val, length);
}

void* memcpy(void* dest, const void* src, size_t n)
{
    return Universal::memcpy(dest, src, n);
}

void* memmove(void* dest, const void* src, size_t n)
{
    return Universal::memmove(dest, src, n);
}

size_t strlen(const char* str)
{
    return Universal::strlen(str);
}
