/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Array.h>
#include <Universal/Heap.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

class MallocManager {
public:
    MallocManager() = default;

    void* allocate(size_t size);
    void deallocate(void* ptr);

private:
    static constexpr size_t kHeapCount = 10;
    static constexpr size_t kHeapChunkSize = 32;
    static constexpr size_t kHeapSize = 4096;

    bool add_heap(size_t heap_size);

    Array<Heap<kHeapChunkSize>, kHeapCount> m_heaps;
    size_t m_heap_count { 0 };
};
