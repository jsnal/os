/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/Graphics/EmulatorVGAGraphicsCard.h>
#include <Kernel/Graphics/VGATextModeGraphicsCard.h>
#include <Universal/Result.h>

class GraphicsManager final {
public:
    GraphicsManager() = default;

    static GraphicsManager& the();

    static SharedPtr<VGATextModeGraphicsCard> init_boot_console();

    Result init_graphics_device(Bus::PCI::Address const&, Bus::PCI::ID const&);

    Result init();

private:
    ArrayList<SharedPtr<EmulatorVGAGraphicsCard>> m_graphics_cards;
};
