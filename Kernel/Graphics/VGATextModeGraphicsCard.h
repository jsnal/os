/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Graphics/GraphicsCard.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

class VGATextModeGraphicsCard : public GraphicsCard {
public:
    enum Color {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGrey = 7,
        DarkGrey = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        LightMagenta = 13,
        LightBrown = 14,
        White = 15,
    };

    static SharedPtr<VGATextModeGraphicsCard> create();

    VGATextModeGraphicsCard();

    ~VGATextModeGraphicsCard() {};

    void scroll(size_t pixels) override;
    void set_color(Color foreground_color, Color background_color);

    void set_pixel(size_t row, size_t column, u32 character) override;
    void put_char(u8 character);
    void put_string(const char*);

    void clear_row(u8 row);
    void clear() override;

    void enable_cursor();
    void disable_cursor();
    void set_cursor(u8 row, u8 column);
    void flush_cursor();

    size_t get_width() override { return m_width; }
    size_t get_height() override { return m_height; }

private:
    Color m_foreground_color { White };
    Color m_background_color { Black };

    u8 m_cursor_row { 0 };
    u8 m_cursor_column { 0 };

    u16* m_buffer;
    u8 m_width { 80 };
    u8 m_height { 25 };
};
