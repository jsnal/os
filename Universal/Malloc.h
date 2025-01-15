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

inline void* malloc(size_t size)
{
    return nullptr;
}

inline void free(void* ptr)
{
    free(ptr);
}
#endif

#ifdef TESTING
#    include <new>
#else
inline void* operator new(size_t, void* ptr)
{
    return ptr;
}

inline void* operator new(size_t size)
{
    return malloc(size);
}

inline void* operator new[](size_t size)
{
    return malloc(size);
}

inline void operator delete(void* ptr)
{
    free(ptr);
}

inline void operator delete[](void* ptr)
{
    free(ptr);
}
#endif
