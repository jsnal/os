/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef KERNEL
#    include <Kernel/kmalloc.h>
#else
#    include <stdlib.h>
#endif

void* malloc(size_t size)
{
#ifdef KERNEL
    return kmalloc(size);
#else
    return malloc(size);
#endif
}

void free(void* ptr)
{
#ifdef KERNEL
    kfree(ptr);
#else
    free(ptr);
#endif
}
