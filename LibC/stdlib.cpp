/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <MallocManager.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static MallocManager s_malloc_manager;

void exit(int status)
{
    fflush(stdout);
    fflush(stderr);
    _exit(status);
}

int abs(int j)
{
    return (j < 0) ? -j : j;
}

void* malloc(size_t size)
{
    return s_malloc_manager.allocate(size);
}

void* calloc(size_t element_count, size_t size)
{
    return nullptr;
}

void free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    s_malloc_manager.deallocate(ptr);
}
