/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Memory/Zone.h>
#include <multiboot.h>

extern u32 g_kernel_end;

class PMM {
public:
    PMM()
    {
    }

    PMM(const multiboot_information_t*);

    Zone kernel_zone() const { return m_kernel_zone; }

    Zone user_zone() const { return m_user_zone; }

private:
    Zone m_kernel_zone;
    Zone m_user_zone;
};
