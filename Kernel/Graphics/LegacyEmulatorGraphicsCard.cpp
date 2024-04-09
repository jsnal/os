/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Graphics/LegacyEmulatorGraphicsCard.h>

LegacyEmulatorGraphicsCard::LegacyEmulatorGraphicsCard(Bus::PCI::ID const& id)
{
}

SharedPtr<LegacyEmulatorGraphicsCard> LegacyEmulatorGraphicsCard::create(Bus::PCI::ID const& id)
{
    return adopt(*new LegacyEmulatorGraphicsCard(id));
}
