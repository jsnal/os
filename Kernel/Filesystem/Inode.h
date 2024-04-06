/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Ext2.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Universal/ArrayList.h>
#include <Universal/Types.h>

class Ext2Filesystem;

class Inode {
public:
    Inode(Ext2Filesystem& fs, ino_t id);

    const Ext2Inode& data() const { return m_raw_data; }

    const ino_t id() const { return m_id; }

    u32 block_group() const;
    u32 index() const;
    u32 block() const;
    u32 number_of_blocks() const;

    void read_single_block_pointer(u32 single_block_pointer, u32& block_index);
    void read_double_block_pointer(u32 double_block_pointer, u32& block_index);
    void read_triple_block_pointer(u32 triple_block_pointer, u32& block_index);
    void read_block_pointers();

    u32 get_block_pointer(u32 index) const;

    Result read(u8* buffer);

private:
    Ext2Inode m_raw_data;
    Ext2Filesystem& m_fs;
    ino_t m_id;

    ArrayList<u32> m_block_pointers;
};
