/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>

extern "C" {

int main(int, char**);
void __stdio_init();

int __errno = 0;

void __libc_init()
{
    __stdio_init();
}

int _start(int argc, char* argv[])
{
    __libc_init();

    int ret = main(argc, argv);
    exit(ret);

    // Should never reach this line
    return 0xdead;
}

void* __dso_handle __attribute__((visibility("hidden")));

void __cxa_pure_virtual()
{
}

void __cxa_atexit()
{
}
}
