/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/DebugConsole.h>
#include <Kernel/kprintf.h>
#include <Universal/PrintFormat.h>

#define KPRINTF_BUFFER_SIZE 256

void kvprintf(const char* format, va_list ag)
{
    char message[KPRINTF_BUFFER_SIZE];
    size_t length;

    length = print_format_buffer(message, KPRINTF_BUFFER_SIZE, format, ag);
    if (length > 0) {
        DebugConsole::the().write(message, length);
    }
}

void kprintf(const char* format, ...)
{
    va_list ap;

    va_start(ap, format);
    kvprintf(format, ap);
    va_end(ap);
}
