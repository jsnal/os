/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>

extern "C" {

int main(int, char**, char**);

int __errno = 0;

int _start(int argc, char* argv[], char* env[])
{
    int ret = main(argc, argv, env);
    exit(ret);

    // Should never reach this line
    return 0xdead;
}
}
