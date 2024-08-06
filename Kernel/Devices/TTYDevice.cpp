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
            *termios_p = m_termios;
            break;
        case TCSETS:
        case TCSETSW:
        case TCSETSF:
            termios_p = reinterpret_cast<termios*>(argp);
            m_termios = *termios_p;
            dump();
            break;
    }

    return 0;
}

#if DEBUG_TTY_DEVICE
void TTYDevice::dump() const
{
    dbgprintf("TTYDevice", "c_iflag=0x%x, c_oflag=0x%x\n", m_termios.c_iflag, m_termios.c_oflag);
    dbgprintf("TTYDevice", "c_cflag=0x%x, c_lflag=0x%x\n", m_termios.c_cflag, m_termios.c_lflag);
    dbgprintf("TTYDevice", "c_cc={\n");
    dbgprintf("TTYDevice", "  VEOF=%u\n", m_termios.c_cc[VEOF]);
    dbgprintf("TTYDevice", "  VEOL=%u\n", m_termios.c_cc[VEOL]);
    dbgprintf("TTYDevice", "  VERASE=%u\n", m_termios.c_cc[VERASE]);
    dbgprintf("TTYDevice", "  VINTR=%u\n", m_termios.c_cc[VINTR]);
    dbgprintf("TTYDevice", "  VKILL=%u\n", m_termios.c_cc[VKILL]);
    dbgprintf("TTYDevice", "  VMIN=%u\n", m_termios.c_cc[VMIN]);
    dbgprintf("TTYDevice", "  VQUIT=%u\n", m_termios.c_cc[VQUIT]);
    dbgprintf("TTYDevice", "  VSTART=%u\n", m_termios.c_cc[VSTART]);
    dbgprintf("TTYDevice", "  VSTOP=%u\n", m_termios.c_cc[VSTOP]);
    dbgprintf("TTYDevice", "  VSUSP=%u\n", m_termios.c_cc[VSUSP]);
    dbgprintf("TTYDevice", "  VTIME=%u\n", m_termios.c_cc[VERASE]);
    dbgprintf("TTYDevice", "}\n");
    dbgprintf("TTYDevice", "c_ispeed=0x%x, c_ospeed=0x%x\n", m_termios.c_ispeed, m_termios.c_ospeed);
}
#endif
