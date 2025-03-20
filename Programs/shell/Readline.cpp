/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Readline.h>
#include <Universal/ArrayList.h>
#include <Universal/BasicString.h>
#include <Universal/Logger.h>
#include <stdio.h>
#include <unistd.h>

Result Readline::redraw()
{
    ArrayList<char, kLineLength + 64> redraw_buffer;

    // Move the cursor to the left-edge of the screen
    redraw_buffer.add_last('\r');

    // Print the prompt
    for (size_t i = 0; i < m_prompt.length(); i++) {
        redraw_buffer.add_last(m_prompt[i]);
    }

    // Print the current buffer
    for (size_t i = 0; i < m_length; i++) {
        redraw_buffer.add_last(m_buffer[i]);
    }

    // Erase to the right of the end of the buffer and move the
    // cursor to the correct location
    String escape_sequence = String::format("\033[K\r\033[%uC", m_position + m_prompt.length());
    for (size_t i = 0; i < escape_sequence.length(); i++) {
        redraw_buffer.add_last(escape_sequence[i]);
    }

    if (write(m_fd_out, redraw_buffer.data(), redraw_buffer.size()) == -1) {
        return Result::Failure;
    }

    return Result::OK;
}

Result Readline::do_move_right()
{
    if (m_length > 0 && m_position != m_length) {
        m_position++;
        return redraw();
    }
    return true;
}

Result Readline::do_move_left()
{
    if (m_length > 0 && m_position > 0) {
        m_position--;
        return redraw();
    }
    return true;
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
        redraw();
    }

    return Result::OK;
}

Result Readline::do_clear()
{
    const char* escape_sequence = "\033[H\033[2J";

    if (write(m_fd_out, escape_sequence, strlen(escape_sequence)) == -1) {
        return Result::Failure;
    }

    return redraw();
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

ResultReturn<const StringView> Readline::raw_read()
{
    if (!enable_raw_mode()) {
        return Result(Result::Failure);
    }

    if (!write(STDOUT_FILENO, m_prompt.str(), m_prompt.length())) {
        return Result(Result::Failure);
    }

    while (true) {
        char c;
        ssize_t nread = ::read(STDIN_FILENO, &c, 1);
        if (nread <= 0) {
            return Result(Result::Failure);
        }

        switch (c) {
            case KeyAction::Escape: {
                char escape_sequence[3];
                if (::read(m_fd_in, escape_sequence, 2) <= 0) {
                    break;
                }

                if (escape_sequence[0] == '[') {
                    switch (escape_sequence[1]) {
                        case 'A':
                            dbgprintln("Shell", "Up arrow!");
                            break;
                        case 'B':
                            dbgprintln("Shell", "Down arrow!");
                            break;
                        case 'C':
                            do_move_right();
                            break;
                        case 'D':
                            do_move_left();
                            break;
                    }
                }
                break;
            }
            case KeyAction::Enter:
                return StringView(m_buffer.data());
            case KeyAction::Backspace:
                do_backspace();
                break;
            case KeyAction::ControlL:
                do_clear();
                break;
            default:
                do_insert(c);
                break;
        }
    }

    return Result(Result::OK);
}

ResultReturn<const StringView> Readline::read()
{
    ASSERT(isatty(m_fd_in));

    auto raw_read_result = raw_read();
    char c = '\n';
    write(m_fd_out, &c, 1);

    return raw_read_result;
}
