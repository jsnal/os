/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Inode.h>

Inode::Inode(Ext2Filesystem& fs, ino_t id)
    : m_fs(fs)
    , m_id(id)
{
}

u32 Inode::block_group() const
{
    return (m_id - 1) / m_fs.super_block().inodes_per_group;
}

u32 Inode::index() const
{
    return (m_id - 1) % m_fs.super_block().inodes_per_group;
}

u32 Inode::block() const
{
    return (index() * m_fs.super_block().inode_size) / m_fs.block_size();
}
