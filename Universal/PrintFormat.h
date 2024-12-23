/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>
#include <stdarg.h>

namespace Universal {

int print_format_buffer(char* str, size_t size, const char* format, va_list ap);

[[gnu::always_inline]] inline int print_format_buffer(char* str, size_t size, const char* format, ...)
{
    size_t length;
    va_list ap;
    va_start(ap, format);
    length = print_format_buffer(str, size, format, ap);
    va_end(ap);

    return length;
}

}

using Universal::print_format_buffer;
