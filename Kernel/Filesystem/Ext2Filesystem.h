/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Filesystem/Ext2.h>
#include <Kernel/Filesystem/Inode.h>
#include <Universal/Result.h>
#include <Universal/Stdlib.h>
#include <Universal/UniquePtr.h>

class Inode;

class Ext2Filesystem {
    friend Inode;

public:
    Ext2Filesystem(UniquePtr<PATADisk> disk)
        : m_disk(move(disk))
    {
    }

    Result init();

    Ext2Superblock& super_block();

    Ext2BlockGroupDescriptor& block_group_descriptor(u32);

    u32 block_size() const { return m_block_size; }

    u32 block_group_count() const { return m_block_group_count; }

    u32 block_pointers_per_block() const;

    Inode* inode(ino_t);

private:
    Result inode_block_and_offset(const Inode& inode, u32& block_index, u32& offset);

    ResultOr<u8*> read_blocks(u32 index, u32 count);

    Result read_blocks(u32 index, u32 count, u8* buffer);

    UniquePtr<PATADisk> m_disk;

    u32 m_block_size { 0 };

    u32 m_block_group_count { 0 };

    u8* m_super_block { nullptr };

    u8* m_block_group_descriptor_table { nullptr };
};
