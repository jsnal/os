/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Universal/Bitmap.h>
#include <Universal/Result.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>

class PhysicalRegion : public ShareCounted<PhysicalRegion> {
public:
    static SharedPtr<PhysicalRegion> create(PhysicalAddress lower, PhysicalAddress upper);

    PhysicalRegion() { }

    void expand(PhysicalAddress lower, PhysicalAddress upper);

    u32 commit();

    ResultOr<PhysicalAddress> allocate_page();

    Result free_page(PhysicalAddress);

private:
    PhysicalRegion(PhysicalAddress lower, PhysicalAddress upper);

    PhysicalAddress m_lower;
    PhysicalAddress m_upper;
    u32 m_total_pages { 0 };
    u32 m_used_pages { 0 };
    u32 m_last_allocated_page { 0 };
    Bitmap m_bitmap;
};
