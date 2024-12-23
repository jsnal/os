/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/kprintf.h>
#include <Universal/PrintFormat.h>
#include <stdio.h>

#define PANIC_BUFFER_SIZE 256

/**
 * @brief freeze the kernel without printing a message
 */
static inline void hang()
{
    asm volatile("cli; \
                  hlt");
}

/**
 * @brief panic the kernel with a message
 */
static inline void panic(const char* format, ...)
{
    char message[PANIC_BUFFER_SIZE];

    va_list ap;
    va_start(ap, format);
    print_format_buffer(message, PANIC_BUFFER_SIZE, format, ap);
    va_end(ap);

    kprintf("*** PANIC ***\n%s", message);

    hang();
}
