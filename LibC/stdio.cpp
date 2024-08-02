/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Printf.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

FILE __stdin = {
    .fd = STDIN_FILENO,
    .options = O_RDONLY,
};

FILE* stdin = &__stdin;

FILE __stdout = {
    .fd = STDOUT_FILENO,
    .options = O_WRONLY,
};

FILE* stdout = &__stdout;

FILE __stderr = {
    .fd = STDERR_FILENO,
    .options = O_WRONLY,
};

FILE* stderr = &__stderr;

int vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    return printf_buffer(str, size, format, ap);
}
