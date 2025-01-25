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

    void* addr = mmap(nullptr, 4096 * 4, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    printf("mmap'd %#08x, errno=%d\n", addr, errno);

    u32* int_addr = (u32*)addr;
    printf("%#08x: %d\n", int_addr, *int_addr);
    *int_addr = 143;
    printf("%#08x: %d\n", int_addr, *int_addr);

    munmap(addr, 1);
    // munmap((void*)0x1005000, 4096 * 2);

    void* addr2 = mmap(nullptr, 4096 * 3, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return 0;
}
