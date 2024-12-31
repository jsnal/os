/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

namespace Memory {

static constexpr u16 kPageSize = 4096;

static constexpr u32 kKernelVirtualBase = 0xC0000000;
static constexpr u32 kKernelPhysicalBase = 0x00100000;
static constexpr size_t kKernelImageMaxLength = 3 * MB;
static constexpr size_t kKernelTemporaryMapLength = kPageSize;
static constexpr u32 kKernelTemporaryMapBase = kKernelVirtualBase + kKernelImageMaxLength;

static constexpr u32 kKernelFreePagesVirtualBase = kKernelVirtualBase + kKernelImageMaxLength + kKernelTemporaryMapLength;
static constexpr size_t kKernelFreePagesLength = (1 * GB) - kKernelImageMaxLength - kKernelTemporaryMapLength;

static constexpr u32 kUserVirtualBase = 0x01000000;
static constexpr size_t kUserVirtualLength = kKernelVirtualBase - kUserVirtualBase;

static constexpr u8 kOutOfMemory = 1;
static constexpr u8 kAddressOutOfRange = 2;
static constexpr u8 kNotPageAligned = 3;

static constexpr inline u32 page_round_up(u32 address)
{
    return ((address + kPageSize - 1) & (~(kPageSize - 1)));
}

static constexpr inline u32 page_round_down(u32 address)
{
    return (address / kPageSize) * kPageSize;
}

static constexpr inline u32 physical_to_virtual(u32 physical_address)
{
    return physical_address + kKernelVirtualBase;
}

static constexpr inline u32 virtual_to_physical(u32 virtual_address)
{
    return virtual_address - kKernelVirtualBase;
}

static inline void flush_tlb()
{
    asm volatile("mov eax, cr3; \
                  mov cr3, eax");
}

static inline void invalidate_page(u32 address)
{
    asm volatile("invlpg [%0]"
                 :
                 : "r"(address));
}

}
