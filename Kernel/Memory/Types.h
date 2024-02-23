/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_IMAGE_VIRTUAL_BASE 0xC0100000

namespace Memory::Types {

constexpr static u8 OutOfMemory = 1;
constexpr static u8 AddressOutOfRange = 2;
constexpr static u8 NotPageAligned = 3;

constexpr static u16 PageSize = 4096;

constexpr inline static u32 page_round_up(u32 address)
{
    return ((address + PageSize - 1) & (~(PageSize - 1)));
}

constexpr static inline u32 physical_to_virtual(u32 physical_address)
{
    return physical_address + KERNEL_VIRTUAL_BASE;
}

constexpr static inline u32 virtual_to_physical(u32 virtual_address)
{
    return virtual_address - KERNEL_VIRTUAL_BASE;
}
}
