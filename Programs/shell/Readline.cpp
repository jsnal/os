/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Readline.h>
#include <stdio.h>
#include <unistd.h>

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
                printf("Backspace!\n");
                break;
            default:
                m_buffer[m_position++] = c;
                write(STDOUT_FILENO, &c, 1);
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
