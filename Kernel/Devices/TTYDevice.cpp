/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/TTYDevice.h>

TTYDevice::TTYDevice(u32 major, u32 minor)
    : CharacterDevice(major, minor)
{
    memset(&m_termios, 0, sizeof(termios));
    m_termios.c_lflag = ECHO | ISIG | ICANON;

    // Add 64 to these values to get ASCII value
    m_termios.c_cc[VEOF] = 4;     // End-of-file (Ctrl-D)
    m_termios.c_cc[VEOL] = 0;     // End-of-line (undefined)
    m_termios.c_cc[VERASE] = 127; // Erase (DEL)
    m_termios.c_cc[VINTR] = 3;    // Interrupt (Ctrl-C)
    m_termios.c_cc[VKILL] = 21;   // Kill line (Ctrl-U)
    m_termios.c_cc[VMIN] = 1;     // Minimum number of characters for noncanonical read
    m_termios.c_cc[VQUIT] = 28;   // Quit (Ctrl-\)
    m_termios.c_cc[VSTART] = 17;  // Start (Ctrl-Q)
    m_termios.c_cc[VSTOP] = 19;   // Stop (Ctrl-S)
    m_termios.c_cc[VSUSP] = 26;   // Suspend (Ctrl-Z)
    m_termios.c_cc[VTIME] = 0;    // Timeout value for noncanonical read
}

ssize_t TTYDevice::read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count)
{
    tty_write(buffer, count);
    return count;
}

ssize_t TTYDevice::write(FileDescriptor&, const u8* buffer, ssize_t count)
{
    return 0;
}
