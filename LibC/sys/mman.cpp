/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/syscall.h>

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    mmap_args args = {
        addr, length, prot, flags, fd, offset
    };
    int ret = syscall(SYS_mmap, (int)&args);
    RETURN_ERRNO(ret, (void*)ret, (void*)-1);
}

int munmap(void* addr, size_t length)
{
    int ret = syscall(SYS_munmap, (int)addr, length);
    RETURN_ERRNO(ret, ret, -1);
}
