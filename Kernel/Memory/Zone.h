/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Bitmap.h>
#include <LibA/Types.h>
#include <Memory/Types.h>

#define KERNEL_ZONE_LENGTH (4 * MB)

class Zone {
public:
    explicit Zone()
        : m_base_address(0)
        , m_length(0)
        , m_bitmap(Bitmap::create())
    {
    }

    Zone(u32 base_address, u32 length, u8* bitmap_address)
        : m_base_address(base_address)
        , m_length(length)
        , m_frames_left(length / PAGE_SIZE)
        , m_bitmap(Bitmap::wrap(bitmap_address, length / PAGE_SIZE / 8))
    {
        m_bitmap.fill(0);
    }

    u32 base_address() const { return m_base_address; }

    Bitmap bitmap() const { return m_bitmap; }

private:
    u32 m_base_address;
    u32 m_length;
    u32 m_frames_left;
    u32 m_last_allocated_frame_index { 0 };
    Bitmap m_bitmap;
};
