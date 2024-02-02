/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _KPRINTF_H_
#define _KPRINTF_H_

#include <stdarg.h>

void kvprintf(const char* format, va_list ag);

void kprintf(const char* format, ...);

#endif
