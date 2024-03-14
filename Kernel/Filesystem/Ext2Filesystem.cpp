/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Universal/Number.h>

Result Ext2Filesystem::init()
{
    auto& super_block = this->super_block();
    if (super_block.signature != EXT2_SUPER_BLOCK_SIGNATURE) {
        return Result::Failure;
    }

    m_block_size = BLOCK_SIZE(super_block.block_size);
    m_block_group_count = ceiling_divide(super_block.total_blocks, super_block.blocks_per_group);

    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "%u inodes, %u blocks\n", super_block.total_inodes, super_block.total_blocks);
    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "block size = %u\n", BLOCK_SIZE(super_block.block_size));
    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "block group count = %u\n", m_block_group_count);

    if (m_block_group_count == 0) {
        dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "No block groups found\n");
        return Result::Failure;
    }

    for (u32 i = 1; i <= m_block_group_count; i++) {
        auto& group = block_group_descriptor(i);
        dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "group %u: [block_bitmap %u, inode_bitmap %u, inode_table: %u]\n",
            i, group.block_usage_bitmap, group.inode_usage_bitmap, group.inode_table);
    }

    return Result::OK;
}

Ext2Superblock& Ext2Filesystem::super_block()
{
    if (m_super_block == nullptr) {
        m_super_block = new u8[1024];
        ASSERT(m_disk->read_sectors(m_super_block, 2, 2).is_ok());
    }

    return *reinterpret_cast<Ext2Superblock*>(m_super_block);
}

Ext2BlockGroupDescriptor& Ext2Filesystem::block_group_descriptor(u32 group_index)
{
    if (m_block_group_descriptor_table == nullptr) {
        u32 blocks_to_read = ceiling_divide(m_block_group_count * (u32)sizeof(Ext2BlockGroupDescriptor), m_block_size);
        u8 first_block = m_block_size == 1024 ? 2 : 1;
        m_block_group_descriptor_table = read_blocks(first_block, blocks_to_read);
    }

    return reinterpret_cast<Ext2BlockGroupDescriptor*>(m_block_group_descriptor_table)[group_index - 1];
}

u8* Ext2Filesystem::read_blocks(u32 index, u32 count)
{
    if (count == 0) {
        return nullptr;
    }

    u8* blocks = new u8[count * m_block_size];
    u32 sectors_to_read = ceiling_divide(count * m_block_size, m_disk->block_size());
    u32 sector = ceiling_divide(index * m_block_size, m_disk->block_size());
    if (m_disk->read_sectors(blocks, sector, sectors_to_read).is_error()) {
        delete[] blocks;
        return nullptr;
    }

    return blocks;
}
