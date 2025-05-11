/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define EOF (-1)
#define BUFSIZ 1024

#define _IOFBF 0 // Fully buffered
#define _IOLBF 1 // Line buffered
#define _IONBF 2 // No buffering

struct __FILE {
    int fd;
    int eof;
    int error;
    int flags;
    char* buffer;
    size_t buffer_index;
    size_t buffer_size;
    char default_buffer[BUFSIZ];
};

typedef struct __FILE FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int fflush(FILE*);

int fputc(int c, FILE* stream);
int putc(int c, FILE* stream);
int putchar(int c);

int fputs(const char* s, FILE* stream);
int puts(const char* s);

int printf(const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);
int udbgprintf(const char* format, ...);

void perror(const char* s);

__END_DECLS
