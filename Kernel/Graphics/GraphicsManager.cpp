/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Graphics/EmulatorVGAGraphicsCard.h>
#include <Kernel/Graphics/GraphicsManager.h>
#include <Universal/Logger.h>

GraphicsManager& GraphicsManager::the()
{
    static GraphicsManager s_the;
    return s_the;
}

GraphicsManager::GraphicsManager()
{
}

Result GraphicsManager::init()
{
    dbgprintf("GraphicsManager", "Initializing the GraphicsManager\n");

    Bus::PCI::Address ide_controller_address = {};
    Bus::PCI::ID ide_controller_id = {};

    Bus::PCI::enumerate_devices([&](Bus::PCI::Address const address, Bus::PCI::ID const id, u16 const type) {
        if (type == Bus::PCI::Type::Display) {
            dbgprintf("GraphicsManager", "Display device found: %x:%x type: %x\n", id.device, id.vendor, type);
            init_graphics_device(address, id);
        }
    });

    return Status::OK;
}

Result GraphicsManager::init_graphics_device(Bus::PCI::Address const& address, Bus::PCI::ID const& id)
{
    SharedPtr<EmulatorVGAGraphicsCard> graphics_card;
    switch (id.vendor) {
        case Bus::PCI::VendorId::LegacyEmulator:
            m_graphics_card = EmulatorVGAGraphicsCard::create(address, id);
            break;
        default:
            dbgprintf("GraphicsManager", "Unable to find graphics card (%x:%x)\n", id.vendor, id.device);
    }

    return Status::OK;
}
