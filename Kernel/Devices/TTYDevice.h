/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/API/termios.h>
#include <Kernel/Devices/CharacterDevice.h>

class TTYDevice : public CharacterDevice {
public:
    TTYDevice(u32 major, u32 minor);

    virtual ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count) override;
    virtual ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count) override;

    virtual size_t tty_write(const u8* buffer, size_t count) = 0;

private:
    termios m_termios;
};
