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
}

ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count)
{
}

ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count)
{
}
