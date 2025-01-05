/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Stdlib.h>

namespace Universal {

void* memset(void* dest, int val, size_t length)
{
    unsigned char* ptr = (unsigned char*)dest;
    while (length-- > 0) {
        *ptr++ = val;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n)
{
    char* d = (char*)dest;
    const char* s = (const char*)src;

    if (d && s) {
        while (n) {
            *(d++) = *(s++);
            --n;
        }
    }

    return dest;
}

void* memmove(void* dest, const void* src, size_t n)
{
    const u8* from = (const u8*)src;
    u8* to = (u8*)dest;

    if (from == to || n == 0) {
        return dest;
    }

    if (to > from && to - from < (int)n) {
        int i;
        for (i = n - 1; i >= 0; i--)
            to[i] = from[i];
        return dest;
    }

    if (from > to && from - to < (int)n) {
        size_t i;
        for (i = 0; i < n; i++)
            to[i] = from[i];
        return dest;
    }

    memcpy(dest, src, n);
    return dest;
}

size_t strlen(const char* str)
{
    size_t length = 0;
    while (str[length]) {
        length++;
    }
    return length;
}

}
