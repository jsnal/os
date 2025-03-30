/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Logger.h>
#include <Universal/PrintFormat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

__BEGIN_DECLS

static FILE __default_streams[3];
FILE* stdin;
FILE* stdout;
FILE* stderr;

static void __stdio_init_stream(FILE& file, int fd, int flags)
{
    file.fd = fd;
    file.flags = flags;
    file.buffer = file.default_buffer;
    file.buffer_size = BUFSIZ;
    file.buffer_index = 0;
}

void __stdio_init()
{
    stdin = &__default_streams[0];
    stdout = &__default_streams[1];
    stderr = &__default_streams[2];
    __stdio_init_stream(*stdin, STDIN_FILENO, isatty(STDIN_FILENO) ? _IOLBF : _IOFBF);
    __stdio_init_stream(*stdout, STDOUT_FILENO, isatty(STDOUT_FILENO) ? _IOLBF : _IOFBF);
    __stdio_init_stream(*stderr, STDERR_FILENO, _IONBF);
}

int fflush(FILE* stream)
{
    if (stream == nullptr) {
        dbgprintln("stdio", "TODO: Handle NULL fflush");
        return EOF;
    }

    if (stream->buffer_index == 0) {
        return 0;
    }

    int ret = write(stream->fd, stream->buffer, stream->buffer_index);
    stream->buffer_index = 0;
    stream->eof = 0;
    if (ret == -1) {
        stream->error = errno;
        return EOF;
    }
    return 0;
}

int fputc(int c, FILE* stream)
{
    if (stream == nullptr || stream->buffer_index >= stream->buffer_size) {
        return EOF;
    }
    stream->buffer[stream->buffer_index++] = c;

    if (stream->buffer_index >= stream->buffer_size || stream->flags == _IONBF || (stream->flags == _IOLBF && c == '\n')) {
        fflush(stream);
    }
    if (stream->eof != 0 || stream->error != 0) {
        return EOF;
    }

    return c;
}

int putc(int c, FILE* stream)
{
    return fputc(c, stream);
}

int putchar(int c)
{
    return putc(c, stdout);
}

int fputs(const char* s, FILE* stream)
{
    if (s == nullptr) {
        return EOF;
    }

    while (*s != 0) {
        if (putc(*s, stream) == EOF) {
            return EOF;
        }
        s++;
    }
    return 1;
}

int puts(const char* s)
{
    if (fputs(s, stdout) == EOF) {
        return EOF;
    }
    if (putchar('\n') == EOF) {
        return EOF;
    }
    return 1;
}

int printf(const char* format, ...)
{
    char buffer[BUFSIZ];

    va_list ap;
    va_start(ap, format);
    int length = vsnprintf(buffer, BUFSIZ, format, ap);
    va_end(ap);

    for (int i = 0; i < length; i++) {
        if (putchar(buffer[i]) == EOF) {
            return EOF;
        }
    }

    return length;
}

int vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    return print_format_buffer(str, size, format, ap);
}

int udbgprintf(const char* format, ...)
{
    char buffer[BUFSIZ];

    va_list ap;
    va_start(ap, format);
    int length = vsnprintf(buffer, BUFSIZ, format, ap);
    va_end(ap);

    (void)syscall(SYS_dbgwrite, (int)buffer, length);
    return length;
}

__END_DECLS
