/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/CharacterDevice.h>
#include <Kernel/POSIX.h>

class TTYDevice : public CharacterDevice {
public:
    TTYDevice(u32 major, u32 minor);

    ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count) override;
    ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count) override;
    int ioctl(FileDescriptor&, uint32_t request, uint32_t* argp) override;

    virtual size_t tty_write(const u8* buffer, size_t count) = 0;

    [[nodiscard]] bool is_signal() const { return m_termios.c_lflag & ISIG; }
    [[nodiscard]] bool is_echo() const { return m_termios.c_lflag & ECHO; }
    [[nodiscard]] bool is_canonical() const { return m_termios.c_lflag & ICANON; }

private:
#if DEBUG_TTY_DEVICE
    void dump() const;
#endif

    termios m_termios;
};
