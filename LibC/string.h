/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);

size_t strlen(const char* str);
int strncmp(const char* s1, const char* s2, size_t n);

__END_DECLS
#endif
