/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/PrintFormat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

__BEGIN_DECLS

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

int printf(const char* format, ...)
{
    // TODO: there should not be a hardcoded buffer limit here
    char buffer[512];

    va_list ap;
    va_start(ap, format);
    int length = vsnprintf(buffer, 512, format, ap);
    va_end(ap);

    write(STDOUT_FILENO, buffer, 512);

    return length;
}

int vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    return print_format_buffer(str, size, format, ap);
}

__END_DECLS
