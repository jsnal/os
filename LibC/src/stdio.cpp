/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibA/Printf.h>

int vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    return printf_buffer(str, size, format, ap);
}
