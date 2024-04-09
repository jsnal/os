/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Universal/Result.h>

class GraphicsManager final {
public:
    GraphicsManager() = default;

    static GraphicsManager& the();

    Result init_graphics_device(Bus::PCI::ID const&);

    Result init();
};
