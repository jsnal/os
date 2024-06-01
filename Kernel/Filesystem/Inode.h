/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Ext2.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Universal/ArrayList.h>
#include <Universal/ShareCounted.h>
#include <Universal/String.h>
#include <Universal/Types.h>

class Ext2Filesystem;
class InodeId;

#define MODE_CHARACTER_DEVICE 0x2000
#define MODE_DIRECTORY 0x4000
#define MODE_BLOCK_DEVICE 0x6000

class Inode : public ShareCounted<Inode> {
public:
    Inode(Ext2Filesystem& fs, ino_t id);

    const Ext2Inode& data() const { return m_raw_data; }

    const ino_t id() const { return m_id; }

    u32 block_group() const;
    u32 index() const;
    u32 block() const;
    u32 number_of_blocks() const;

    bool is_directory() const { return (m_raw_data.mode & 0xF000) == MODE_DIRECTORY; };
    bool is_character_device() const { return (m_raw_data.mode & 0xF000) == MODE_CHARACTER_DEVICE; };
    bool is_block_device() const { return (m_raw_data.mode & 0xF000) == MODE_BLOCK_DEVICE; };
    bool is_device() const { return is_block_device() || is_character_device(); }

    u32 major_device_number() const;
    u32 minor_device_number() const;

    ResultReturn<InodeId> find(const String&);

    void read_block_pointers();

    u32 get_block_pointer(u32 index) const;

    ResultReturn<ssize_t> read(size_t start, size_t length, u8* buffer);

private:
    void read_single_block_pointer(u32 single_block_pointer, u32& block_index);
    void read_double_block_pointer(u32 double_block_pointer, u32& block_index);
    void read_triple_block_pointer(u32 triple_block_pointer, u32& block_index);

    Ext2Inode m_raw_data;
    Ext2Filesystem& m_fs;
    ino_t m_id;

    ArrayList<u32> m_block_pointers;
};
