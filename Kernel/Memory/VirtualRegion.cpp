/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/VirtualRegion.h>

VirtualRegion::VirtualRegion()
{
    MemoryManager::the().add_virtual_region(*this);
}

VirtualRegion::~VirtualRegion()
{
    MemoryManager::the().remove_virtual_region(*this);
}
