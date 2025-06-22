/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Ext2.h>
#include <Kernel/Filesystem/Inode.h>
#include <Universal/ArrayList.h>
#include <Universal/BasicString.h>
#include <Universal/Types.h>

class Ext2Filesystem;
class InodeId;

#define MODE_CHARACTER_DEVICE 0x2000
#define MODE_DIRECTORY 0x4000
#define MODE_BLOCK_DEVICE 0x6000

class Ext2Inode : public Inode {
public:
    Ext2Inode(Ext2Filesystem& fs, ino_t id);

    const Ext2RawInode& data() const { return m_raw_data; }

    u32 block_group();
    u32 index();
    u32 block();
    u32 number_of_blocks() const;

    void read_block_pointers();
    u32 get_block_pointer(u32 index) const;

    inline Ext2Filesystem& ext2_fs() { return reinterpret_cast<Ext2Filesystem&>(fs()); }
    inline const Ext2Filesystem& ext2_fs() const { return reinterpret_cast<const Ext2Filesystem&>(fs()); }

    ResultAnd<InodeId> find(const String&) override;
    ResultAnd<ssize_t> read(size_t start, size_t length, u8* buffer, FileDescriptor&) override;
    ResultAnd<ssize_t> write(size_t start, size_t length, u8* buffer, FileDescriptor&) override;
    ResultAnd<int> fstat(stat&, FileDescriptor&) override;
    void open(FileDescriptor&, int flags) override;
    void close(FileDescriptor&) override;

private:
    void read_single_block_pointer(u32 single_block_pointer, u32& block_index);
    void read_double_block_pointer(u32 double_block_pointer, u32& block_index);
    void read_triple_block_pointer(u32 triple_block_pointer, u32& block_index);

    Ext2RawInode m_raw_data;
    ArrayList<u32> m_block_pointers;
};
