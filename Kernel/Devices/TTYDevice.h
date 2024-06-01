/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/CharacterDevice.h>

class TTYDevice : public CharacterDevice {
public:
    TTYDevice(u32 major, u32 minor);

private:
};
