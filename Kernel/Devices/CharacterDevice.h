/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/Device.h>

class CharacterDevice : public Device {
public:
    virtual bool is_character_device() override final { return true; }

protected:
    CharacterDevice(u32 major, u32 minor)
        : Device(major, minor)
    {
    }
};
