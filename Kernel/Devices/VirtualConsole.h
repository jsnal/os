/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/TTYDevice.h>

class VirtualConsole : public TTYDevice {
public:
    VirtualConsole(u32 major, u32 minor)
        : TTYDevice(major, minor)
    {
    }

    virtual size_t tty_write(const u8* buffer, size_t count) = 0;
};
