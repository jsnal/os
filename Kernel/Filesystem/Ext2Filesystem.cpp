/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Ext2Filesystem.h>

Result Ext2Filesystem::init()
{
    auto& super_block = this->super_block();
    if (super_block.signature != EXT2_SUPER_BLOCK_SIGNATURE) {
        return Result::Failure;
    }

    return Result::OK;
}

Ext2Superblock& Ext2Filesystem::super_block()
{
    if (m_super_block == nullptr) {
        m_super_block = new u8[1024];
        ASSERT(m_disk->read_block(m_super_block, 2).is_ok());
        ASSERT(m_disk->read_block(m_super_block + 512, 3).is_ok());
    }

    return *reinterpret_cast<Ext2Superblock*>(m_super_block);
}
