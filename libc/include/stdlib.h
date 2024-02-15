/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

int abs(int j);

char* itoa(char* dest, size_t size, int a, int base);

__END_DECLS

#endif
