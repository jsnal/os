/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>

class LegacyEmulatorGraphicsCard : public ShareCounted<LegacyEmulatorGraphicsCard> {
public:
    static SharedPtr<LegacyEmulatorGraphicsCard> create(Bus::PCI::ID const&);

private:
    LegacyEmulatorGraphicsCard(Bus::PCI::ID const&);
};
