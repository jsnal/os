/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/CharacterDevice.h>

class RandomDevice final : public CharacterDevice {
public:
    RandomDevice();

private:
    u32 next_random();

    ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count) override;
    ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count) override;
    int fstat(FileDescriptor&, stat&) override;
    int ioctl(FileDescriptor&, uint32_t request, uint32_t* argp) override;

    u32 m_seed { 0 };
};
