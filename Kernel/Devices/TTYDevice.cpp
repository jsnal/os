/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/TTYDevice.h>
#include <LibC/sys/ioctl_defines.h>

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
    tty_write(buffer, count);
    return 0;
}

int TTYDevice::ioctl(FileDescriptor&, uint32_t request, uint32_t* argp)
{
    termios* termios_p;
    switch (request) {
        case TCGETS:
            termios_p = reinterpret_cast<termios*>(argp);
            termios_p->c_iflag = m_termios.c_iflag;
            termios_p->c_oflag = m_termios.c_oflag;
            termios_p->c_cflag = m_termios.c_cflag;
            termios_p->c_lflag = m_termios.c_lflag;
            memcpy(termios_p->c_cc, m_termios.c_cc, NCCS);
            termios_p->c_ispeed = m_termios.c_ispeed;
            termios_p->c_ospeed = m_termios.c_ospeed;
            break;
    }

    return 0;
}
