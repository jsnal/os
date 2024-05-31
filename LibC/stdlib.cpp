/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <unistd.h>

void exit(int status)
{
    _exit(status);
}

int abs(int j)
{
    return (j < 0) ? -j : j;
}
