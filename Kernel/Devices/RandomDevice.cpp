/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/RandomDevice.h>

RandomDevice::RandomDevice()
    : CharacterDevice(1, 9)
{
}

u32 RandomDevice::next_random()
{
    m_seed = (1664525 * m_seed + 1013904223) % 4294967296;
    return m_seed;
}

ssize_t RandomDevice::read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count)
{
    for (ssize_t i = 0; i < count; i++) {
        buffer[i] = next_random();
    }

    return count;
}

ssize_t RandomDevice::write(FileDescriptor&, const u8* buffer, ssize_t count)
{
    return 0;
}

int RandomDevice::ioctl(FileDescriptor&, uint32_t request, uint32_t* argp)
{
    return 0;
}
