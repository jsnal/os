/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/DebugConsole.h>
#include <Kernel/IO.h>
#include <stdarg.h>

#define COM1 0x3f8
#define LINE_STATUS 0x3FD

// Print out to Bochs console through 0xE9
#define CONSOLE_ENABLE_BOCHS

DebugConsole& DebugConsole::the()
{
    static DebugConsole s_the;
    return s_the;
}

DebugConsole::DebugConsole()
{
}

bool DebugConsole::enable()
{
    IO::outb(COM1 + 1, 0x00);
    IO::outb(COM1 + 3, 0x80);
    IO::outb(COM1 + 0, 0x03);
    IO::outb(COM1 + 1, 0x00);
    IO::outb(COM1 + 3, 0x03);
    IO::outb(COM1 + 2, 0xC7);
    IO::outb(COM1 + 4, 0x0B);
    IO::outb(COM1 + 4, 0x1E);
    IO::outb(COM1 + 0, 0xAE);

    if (IO::inb(COM1 + 0) != 0xAE) {
        return false;
    }

    IO::outb(COM1 + 4, 0x0F);
    m_is_enabled = true;
    return true;
}

void DebugConsole::put_char(char c)
{
    if (!m_is_enabled) {
        return;
    }

    while ((IO::inb(LINE_STATUS) & 0x20) == 0)
        ;

#ifdef CONSOLE_ENABLE_BOCHS
    IO::outb(0xe9, c);
#endif

    IO::outb(COM1, c);
}

void DebugConsole::write(const char* string, size_t length)
{
    if (!m_boot_console.is_null()) {
        m_boot_console->put_string(string, length);
    }

    for (size_t i = 0; i < length; i++) {
        put_char(string[i]);
    }
}
