/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Array.h>
#include <Universal/Result.h>
#include <Universal/StringView.h>
#include <termios.h>

class Readline {
public:
    Readline(int fd_in, int fd_out)
        : m_fd_in(fd_in)
        , m_fd_out(fd_out)
    {
    }

    ResultReturn<const StringView> read(const StringView& prompt);

private:
    enum KeyAction {
        Escape = 0x1B,
        Backspace = 0x08,
        Enter = 0x0A,
    };

    static constexpr size_t kLineLength = 256;

    Result redraw();
    Result do_backspace();
    Result do_insert(char);

    bool enable_raw_mode();
    ResultReturn<const StringView> raw_read(const StringView& prompt);

    Array<char, kLineLength> m_buffer;
    size_t m_length { 0 };
    size_t m_position { 0 };

    struct termios m_original_termios;
    int m_fd_in { -1 };
    int m_fd_out { -1 };
};
