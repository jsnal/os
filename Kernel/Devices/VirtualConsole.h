/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/KeyboardDevice.h>
#include <Kernel/Devices/TTYDevice.h>
#include <Kernel/Graphics/GraphicsManager.h>
#include <Universal/Array.h>

class VirtualConsole final : public TTYDevice
    , public KeyboardListener {
public:
    VirtualConsole(u32 const minor);

    virtual ~VirtualConsole() override;

    void set_focused(bool);

    size_t tty_write(const u8* buffer, size_t count) override;
    void tty_echo(char) override;

    void handle_key_event(KeyEvent) override;

private:
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

    enum EscapeSequenceState {
        Text,
        Escape,
        ControlSequenceIntroducer,
        Parameter,
    };

    void scroll(size_t rows);
    void set_color(Color foreground_color, Color background_color);
    void set_cell(size_t row, size_t column, u32 character);

    void handle_escape_sequence(char command);

    void put_escape_sequence(char);
    void put_char(char);
    size_t put_string(const char* string, size_t length);

    void clear_row(u8 row);
    void clear();

    void enable_cursor();
    void disable_cursor();
    void set_cursor(u8 row, u8 column);
    void flush_cursor();

    Color m_foreground_color { White };
    Color m_background_color { Black };

    EscapeSequenceState m_escape_sequence_state { Text };

    bool m_focused { false };

    u8 m_cursor_row { 0 };
    u8 m_cursor_column { 0 };

    u16* m_buffer { reinterpret_cast<u16*>(0x000B8000) };
    u8 m_width { 80 };
    u8 m_height { 25 };
};
