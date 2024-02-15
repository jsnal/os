/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

int vsnprintf(char* str, size_t size, const char* format, va_list ap);

__END_DECLS

#endif
