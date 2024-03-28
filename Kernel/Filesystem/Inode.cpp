/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Inode.h>
#include <Universal/Stdlib.h>

Inode::Inode(Ext2Filesystem& fs, ino_t id)
    : m_fs(fs)
    , m_id(id)
{
    auto& super_block = m_fs.super_block();
    auto& block_group_descriptor = m_fs.block_group_descriptor(block_group());

    u32 inodes_per_block = BLOCK_SIZE(super_block.block_size) / super_block.inode_size;
    u32 offset = (index() % inodes_per_block) * super_block.inode_size;
    auto result = m_fs.read_blocks(block_group_descriptor.inode_table + block(), 1);
    if (result.is_ok()) {
        memcpy(&m_raw_data, result.value() + offset, sizeof(Ext2Inode));
        delete result.value();
    }
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

u32 Inode::get_block_pointer(u32 index) const
{
    // if (index > )
    return 0;
}

Result Inode::read(u8* buffer)
{
    dbgprintf("Inode", "reading: %d\n", m_raw_data.block_pointers[0]);
    m_fs.read_blocks(m_raw_data.block_pointers[0], 1, buffer);

    return Result::OK;
}
