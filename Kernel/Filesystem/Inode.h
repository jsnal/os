/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Ext2.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Universal/Types.h>

class Ext2Filesystem;

class Inode {
public:
    Inode(Ext2Filesystem&, ino_t);

    u32 block_group() const;
    u32 index() const;
    u32 block() const;

private:
    Ext2Inode m_raw_data;
    Ext2Filesystem& m_fs;
    ino_t m_id;
};
