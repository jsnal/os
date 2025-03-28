/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Bitmap.h>
#include <Universal/Types.h>

struct AllocationHeader {
    size_t size;
    u8 data[0];
};

template<size_t ChunkSize>
class Heap {
public:
    Heap(u8* heap, size_t heap_size)
        : m_total_chunks((heap_size - calculate_minimum_bytes(heap_size / ChunkSize)) / ChunkSize)
        , m_chunks(heap)
        , m_bitmap(heap + m_total_chunks * ChunkSize, m_total_chunks)
    {
    }

    void* allocate(size_t size)
    {
        size_t allocation_size = size + sizeof(AllocationHeader);
        size_t chunks_needed = (allocation_size + ChunkSize - 1) / ChunkSize;

        if (chunks_needed > free_chunks()) {
            return nullptr;
        }

        u32 chunks_found = 0;
        size_t chunk_start_index = 0;
        for (size_t i = 0; i < m_total_chunks && chunks_found < chunks_needed; i++) {
            if (!m_bitmap.get(i)) {
                if (chunks_found == 0) {
                    chunk_start_index = i;
                }
                chunks_found++;
            } else {
                chunks_found = 0;
            }
        }

        if (chunks_found != chunks_needed) {
            return nullptr;
        }

        for (size_t i = chunk_start_index; i < chunk_start_index + chunks_needed; i++) {
            m_bitmap.set(i, true);
        }

        auto* allocation_header = (AllocationHeader*)((u8*)m_chunks + chunk_start_index * ChunkSize);
        allocation_header->size = chunks_needed;

        m_allocated_chunks = m_allocated_chunks + chunks_needed;
        return allocation_header->data;
    }

    bool deallocate(void* ptr)
    {
        if (ptr == nullptr) {
            return false;
        }

        auto* allocation_header = (AllocationHeader*)((u8*)ptr - sizeof(AllocationHeader));
        if ((u8*)allocation_header < m_chunks || (u8*)allocation_header > m_chunks + m_total_chunks * ChunkSize) {
            return false;
        }

        size_t chunk_start_index = ((u8*)allocation_header - m_chunks) / ChunkSize;
        size_t chunk_end_index = chunk_start_index + allocation_header->size;

        if (!m_bitmap.get(chunk_start_index)) {
            return false;
        }

        for (size_t i = chunk_start_index; i < chunk_end_index; i++) {
            m_bitmap.set(i, false);
        }
        m_allocated_chunks = m_allocated_chunks - allocation_header->size;

        return true;
    }

    constexpr size_t chunk_size() const { return ChunkSize; }
    size_t total_chunks() const { return m_total_chunks; }
    size_t allocated_chunks() const { return m_allocated_chunks; }
    size_t free_chunks() const { return m_total_chunks - m_allocated_chunks; }

private:
    size_t m_total_chunks { 0 };
    size_t m_allocated_chunks { 0 };
    u8* m_chunks { nullptr };
    Bitmap m_bitmap;
};
