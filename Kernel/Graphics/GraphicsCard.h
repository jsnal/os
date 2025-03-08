/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/RefCounted.h>
#include <Universal/Types.h>

class GraphicsCard : public RefCounted<GraphicsCard> {
public:
    GraphicsCard() = default;

    virtual ~GraphicsCard() = default;

    [[nodiscard]] virtual bool is_text_mode() const { return false; }

    virtual void scroll(size_t pixels) = 0;
    virtual void set_pixel(size_t x, size_t y, u32 value) = 0;
    virtual void clear() = 0;

    virtual size_t get_width() = 0;
    virtual size_t get_height() = 0;
};
