/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/BlockDevice.h>
#include <Kernel/Filesystem/Ext2.h>
#include <Kernel/Filesystem/Filesystem.h>
#include <Universal/Logger.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/Stdlib.h>
#include <Universal/UniquePtr.h>

class Inode;
class InodeId;

class Ext2Filesystem : public Filesystem {
    friend Inode;

public:
    Ext2Filesystem(UniquePtr<BlockDevice> disk)
        : Filesystem()
        , m_disk(move(disk))
    {
    }

    ~Ext2Filesystem() { dbgprintf("Ext2Filesystem", "Freeing!\n"); }

    Result init() override;

    Ext2RawSuperblock& super_block();

    Ext2RawBlockGroupDescriptor& block_group_descriptor(u32);

    u32 block_size() const { return m_block_size; }
    u32 block_group_count() const { return m_block_group_count; }
    u32 block_pointers_per_block() const;

    InodeId root_inode() const override;
    SharedPtr<Inode> inode(const InodeId&) override;

    Result read_block(u32 index, u8* buffer);
    Result read_blocks(u32 index, u32 count, u8* buffer);

private:
    Result inode_block_and_offset(const Inode& inode, u32& block_index, u32& offset);

    UniquePtr<BlockDevice> m_disk;

    u32 m_block_size { 0 };

    u32 m_block_group_count { 0 };

    u8* m_super_block { nullptr };

    u8* m_block_group_descriptor_table { nullptr };
};
