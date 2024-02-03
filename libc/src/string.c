/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>
#include <string.h>

void* memset(void* dest, int val, size_t length)
{
    unsigned char* ptr = dest;

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
    const uint8_t* from = (const uint8_t*)src;
    uint8_t* to = (uint8_t*)dest;

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

char* itoa(char* dest, size_t size, int a, int base)
{
    char buffer[sizeof(a) * CHAR_BIT + 1 + 1];
    static const char digits[36] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (base < 2 || base > 36) {
        return NULL;
    }

    char* p = &buffer[sizeof(buffer) - 1];
    *p = '\0';

    int an = a < 0 ? a : -a;

    do {
        *(--p) = digits[-(an % base)];
        an /= base;
    } while (an);

    if (a < 0) {
        *(--p) = '-';
    }

    size_t str_size = &buffer[sizeof(buffer)] - p;
    if (str_size > size) {
        return NULL;
    }

    return memcpy(dest, p, str_size);
}
