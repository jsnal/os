/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Boot/multiboot.h>
#include <Kernel/Memory/Zone.h>
#include <Kernel/kmalloc.h>

extern u32 g_kernel_end;

class PMM {
public:
    PMM()
    {
    }

    PMM(const multiboot_information_t*);

    Region<PhysicalAddress>& kernel_region() const { return *m_kernel_region; }
    Region<PhysicalAddress>& user_region() const { return *m_user_region; }

private:
    Region<PhysicalAddress>* m_kernel_region;
    Region<PhysicalAddress>* m_user_region;
};
