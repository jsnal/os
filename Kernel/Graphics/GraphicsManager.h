/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/Graphics/GraphicsCard.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>

class GraphicsManager final {
public:
    GraphicsManager();

    static GraphicsManager& the();

    Result init();

    [[nodiscard]] SharedPtr<GraphicsCard> text_mode_graphics() const { return m_text_mode_graphics; }

    [[nodiscard]] SharedPtr<GraphicsCard> graphics_card() const { return m_graphics_card; }

private:
    Result init_graphics_device(Bus::PCI::Address const&, Bus::PCI::ID const&);

    SharedPtr<GraphicsCard> m_text_mode_graphics;

    SharedPtr<GraphicsCard> m_graphics_card;

    // TODO: We may be able to have multiple graphics cards and drivers
    // ArrayList<SharedPtr<EmulatorVGAGraphicsCard>> m_graphics_cards;
};
