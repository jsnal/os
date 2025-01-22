/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Readline.h>
#include <stdio.h>
#include <unistd.h>

Result Readline::redraw()
{
    Array<char, kLineLength + 64> redrawn_buffer;
    redrawn_buffer[0] = '\r';

    if (write(m_fd_out, redrawn_buffer.ptr(), 32) == -1) {
        return Result::Failure;
    }

    return Result::OK;
}

Result Readline::do_backspace()
{
    if (m_position > 0 && m_length > 0) {
        memmove(m_buffer.ptr() + m_position - 1, m_buffer.ptr() + m_position, m_length - m_position);
        m_position--;
        m_buffer[--m_length] = '\0';
    }

    return redraw();
}

Result Readline::do_insert(char c)
{
    if (m_length >= m_buffer.size()) {
        return Result::Failure;
    }

    if (m_length != m_position) {
        memmove(m_buffer.ptr() + m_position + 1, m_buffer.ptr() + m_position, m_length - m_position);
    }

    m_buffer[m_position++] = c;
    m_buffer[++m_length] = '\0';

    if (m_length == m_position) {
        write(m_fd_out, &c, 1);
    } else {
        // Redraw
    }

    return Result::OK;
}

bool Readline::enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &m_original_termios);

    // Input modes: no break, no CR to NL, no parity check, no strip char,
    // no start/stop output control.
    m_original_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // Output modes: disable post processing
    m_original_termios.c_oflag &= ~(OPOST);

    // Control modes - set 8 bit chars
    m_original_termios.c_cflag |= (CS8);

    // Local modes: echoing off, canonical off, no extended functions,
    // no signal chars (^Z,^C)
    m_original_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Control chars: set return condition: min number of bytes and timer.
    // We want read to return every single byte, without timeout.
    m_original_termios.c_cc[VMIN] = 1;

    // 1 byte, no timer
    m_original_termios.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_original_termios) < 0) {
        return false;
    }

    return true;
}

ResultReturn<const StringView> Readline::raw_read(const StringView& prompt)
{
    if (!enable_raw_mode()) {
        return Result(Result::Failure);
    }

    if (!write(STDOUT_FILENO, prompt.str(), prompt.length())) {
        return Result(Result::Failure);
    }

    while (true) {
        char c;
        ssize_t nread = ::read(STDIN_FILENO, &c, 1);
        if (nread <= 0) {
            return Result(Result::Failure);
        }

        switch (c) {
            case KeyAction::Enter:
                return StringView(m_buffer.data());
            case KeyAction::Backspace:
                do_backspace();
                break;
            default:
                do_insert(c);
                break;
        }
    }

    return Result(Result::Failure);
}

ResultReturn<const StringView> Readline::read(const StringView& prompt)
{
    ASSERT(isatty(m_fd_in));

    auto raw_read_result = raw_read(prompt);
    char c = '\n';
    write(m_fd_out, &c, 1);

    return raw_read_result;
}
