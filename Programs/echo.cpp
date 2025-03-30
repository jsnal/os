/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>

int main(int argc, char** argv)
{
    printf("Running echo, argc = %d, argv = %d, envp = %d\n", argc, (int)argv);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = '%s'\n", i, argv[i]);
    }

    return 0;
}
