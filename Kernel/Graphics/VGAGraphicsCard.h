/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

class VGAGraphicsCard {
public:
    VGAGraphicsCard() {};

    ~VGAGraphicsCard() {};

    virtual void scroll(size_t pixels) = 0;
    virtual void set_pixel(size_t x, size_t y, u32 value) = 0;
    virtual void clear(u32 color) = 0;

    virtual size_t get_width() = 0;
    virtual size_t get_height() = 0;
};
