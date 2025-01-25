/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <MallocManager.h>
#include <stdio.h>
#include <sys/mman.h>

bool MallocManager::add_heap(size_t heap_size)
{
    void* heap = mmap(nullptr, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (heap == nullptr) {
        return false;
    }

    m_heaps[m_heap_count++] = Heap<kHeapChunkSize>(static_cast<u8*>(heap), heap_size);
    return true;
}

void* MallocManager::allocate(size_t size)
{
    for (size_t i = 0; i < m_heap_count; i++) {
        void* ptr = m_heaps[i].allocate(size);
        if (ptr != nullptr) {
            return ptr;
        }
    }

    if (!add_heap(kHeapSize)) {
        return nullptr;
    }

    return m_heaps[m_heap_count - 1].allocate(size);
}

void MallocManager::deallocate(void* ptr)
{
    for (size_t i = 0; i < m_heap_count; i++) {
        if (m_heaps[i].deallocate(ptr)) {
            break;
        }
    }
}
