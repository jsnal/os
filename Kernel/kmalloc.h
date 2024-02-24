/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Kernel/Memory/Heap.h>
#include <Universal/Types.h>

#define KMALLOC_HEAP_CHUNK_SIZE 32

#define NEW_FOREVER \
public:             \
    void* operator new(size_t size) { return kmalloc_forever(size); }

class KmallocTracker {
public:
    KmallocTracker(u8* initial_heap, size_t initial_heap_size)
        : m_heap(initial_heap, initial_heap_size)
    {
    }

    const Heap<KMALLOC_HEAP_CHUNK_SIZE> heap() const { return m_heap; }
    Heap<KMALLOC_HEAP_CHUNK_SIZE> heap() { return m_heap; }

private:
    void add_heap(u8* heap, size_t heap_size);

    Heap<KMALLOC_HEAP_CHUNK_SIZE> m_heap;
};

void kmalloc_init();

void* kmalloc(size_t);

void kmalloc_free(void*);

void* kmalloc_forever(size_t);