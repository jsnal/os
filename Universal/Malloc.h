/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef KERNEL
#    include <Kernel/kmalloc.h>

inline void* malloc(size_t size)
{
    return kmalloc(size);
}

inline void free(void* ptr)
{
    kfree(ptr);
}

#else
#    include <stdlib.h>
#endif
