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
    static ResultOr<Inode*> create(Ext2Filesystem&, ino_t);

    const Ext2Inode& data() const { return m_raw_data; }

    u32 block_group() const;
    u32 index() const;
    u32 block() const;

private:
    Inode(Ext2Filesystem& fs, ino_t id)
        : m_fs(fs)
        , m_id(id)
    {
    }

    Ext2Inode m_raw_data;
    Ext2Filesystem& m_fs;
    ino_t m_id;
};
