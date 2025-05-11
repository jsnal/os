/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Stdlib.h>
#include <Universal/Types.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

const char* const sys_errlist[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "I/O error",
    "Bad file descriptor number",
    "Out of memory",
    "No child processes",
    "Bad address",
    "Not a directory",
    "Invalid argument",
    "File error"
};

void* memset(void* dest, int val, size_t length)
{
    return Universal::memset(dest, val, length);
}

void* memcpy(void* dest, const void* src, size_t n)
{
    return Universal::memcpy(dest, src, n);
}

void* memmove(void* dest, const void* src, size_t n)
{
    return Universal::memmove(dest, src, n);
}

size_t strlen(const char* str)
{
    return Universal::strlen(str);
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    return Universal::strncmp(s1, s2, n);
}

char* strerror(int errnum)
{
    if (errnum >= EMAXERRNO) {
        return const_cast<char*>("Unknown error");
    }
    return const_cast<char*>(sys_errlist[errnum]);
}
