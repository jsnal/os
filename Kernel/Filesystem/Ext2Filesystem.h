/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Filesystem/Ext2.h>
#include <Universal/Stdlib.h>
#include <Universal/UniquePtr.h>

class Ext2Filesystem {
public:
    Ext2Filesystem(UniquePtr<PATADisk> disk)
        : m_disk(move(disk))
    {
    }

    Result init();

    Ext2Superblock& super_block();

private:
    UniquePtr<PATADisk> m_disk;

    u8* m_super_block { nullptr };
};
