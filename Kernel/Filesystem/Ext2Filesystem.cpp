/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Kernel/Filesystem/Ext2Inode.h>
#include <Kernel/Filesystem/InodeId.h>
#include <Universal/Number.h>

#define DEBUG_EXT2_FILESYSTEM 0

Result Ext2Filesystem::init()
{
    auto& super_block = this->super_block();
    if (super_block.signature != EXT2_SUPER_BLOCK_SIGNATURE) {
        return Status::Failure;
    }

    m_block_size = BLOCK_SIZE(super_block.block_size);
    m_block_group_count = ceiling_divide(super_block.total_blocks, super_block.blocks_per_group);

    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "%u inodes, %u blocks\n", super_block.total_inodes, super_block.total_blocks);
    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "block size = %u\n", BLOCK_SIZE(super_block.block_size));
    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "block group count = %u\n", m_block_group_count);
    dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "first inode = %u\n", super_block.first_inode);

    if (m_block_group_count == 0) {
        dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "No block groups found\n");
        return Status::Failure;
    }

    for (u32 i = 0; i < m_block_group_count; i++) {
        auto& group = block_group_descriptor(i);
        dbgprintf_if(DEBUG_EXT2_FILESYSTEM, "Ext2Filesystem", "group %u: [block_bitmap %u, inode_bitmap %u, inode_table: %u]\n",
            i, group.block_usage_bitmap, group.inode_usage_bitmap, group.inode_table);
    }

    return Status::OK;
}

Ext2RawSuperblock& Ext2Filesystem::super_block()
{
    if (m_super_block.is_null()) {
        m_super_block = ByteBuffer(1024);
        ASSERT(m_disk->read_blocks(2, 2, m_super_block.data()).is_ok());
    }

    return *reinterpret_cast<Ext2RawSuperblock*>(m_super_block.data());
}

Ext2RawBlockGroupDescriptor& Ext2Filesystem::block_group_descriptor(u32 group_index)
{
    if (m_block_group_descriptor_table.is_null()) {
        u32 blocks_to_read = ceiling_divide(m_block_group_count * static_cast<u32>(sizeof(Ext2RawBlockGroupDescriptor)), m_block_size);
        u8 first_block = m_block_size == 1024 ? 2 : 1;

        m_block_group_descriptor_table = ByteBuffer(m_block_size * blocks_to_read);
        MUST(read_blocks(first_block, blocks_to_read, m_block_group_descriptor_table.data()));
    }

    return reinterpret_cast<Ext2RawBlockGroupDescriptor*>(m_block_group_descriptor_table.data())[group_index];
}

u32 Ext2Filesystem::block_pointers_per_block() const
{
    return block_size() / sizeof(32);
}

InodeId Ext2Filesystem::root_inode() const
{
    return InodeId(id(), EXT2_ROOT_INO);
}

SharedPtr<Inode> Ext2Filesystem::inode(const InodeId& inode)
{
    return make_shared_ptr<Ext2Inode>(*this, inode.id());
}

Result Ext2Filesystem::read_block(u32 index, u8* buffer)
{
    return read_blocks(index, 1, buffer);
}

Result Ext2Filesystem::read_blocks(u32 index, u32 count, u8* buffer)
{
    if (buffer == nullptr) {
        return Status::Failure;
    }

    if (count == 0) {
        return Status::OK;
    }

    u32 disk_blocks_to_read = ceiling_divide(count * m_block_size, m_disk->block_size());
    u32 disk_block = ceiling_divide(index * m_block_size, m_disk->block_size());

    return m_disk->read_blocks(disk_block, disk_blocks_to_read, buffer);
}

Result Ext2Filesystem::write_block(u32 index, const u8* buffer)
{
    return write_blocks(index, 1, buffer);
}

Result Ext2Filesystem::write_blocks(u32 index, u32 count, const u8* buffer)
{
    if (buffer == nullptr) {
        return Status::Failure;
    }

    if (count == 0) {
        return Status::OK;
    }

    u32 disk_blocks_to_write = ceiling_divide(count * m_block_size, m_disk->block_size());
    u32 disk_block = ceiling_divide(index * m_block_size, m_disk->block_size());

    return m_disk->write_blocks(disk_block, disk_blocks_to_write, buffer);
}
