/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Inode.h>
#include <Universal/Stdlib.h>

ResultOr<Inode*> Inode::create(Ext2Filesystem& fs, ino_t id)
{
    Inode* inode = new Inode(fs, id);
    auto result = fs.read_blocks(inode->block(), 1);
    if (result.is_error()) {
        delete inode;
        return result.error();
    }

    memcpy(reinterpret_cast<u8*>(&inode->m_raw_data), result.value(), sizeof(Ext2Inode));

    return inode;
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
