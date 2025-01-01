/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int id = getuid();
    printf("uid=%u\n", id);
    return 0;
}
