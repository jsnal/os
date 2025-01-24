/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int id = getuid();
    printf("uid=%u\n", id);

    void* addr = mmap(nullptr, 512, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    printf("mmap'd %#08x, errno=%d\n", addr, errno);

    return 0;
}
