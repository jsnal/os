/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Kernel/Filesystem/Ext2Inode.h>
#include <Kernel/Filesystem/InodeId.h>
#include <Kernel/POSIX.h>
#include <LibC/errno_defines.h>
#include <Universal/Number.h>
#include <Universal/Stdlib.h>

#define DEBUG_INODE 0

Ext2Inode::Ext2Inode(Ext2Filesystem& fs, ino_t id)
    : Inode(fs, id)
{
    m_block_pointers = ArrayList<u32>();

    auto& super_block = ext2_fs().super_block();
    auto& block_group_descriptor = ext2_fs().block_group_descriptor(block_group());

    u32 inodes_per_block = BLOCK_SIZE(super_block.block_size) / super_block.inode_size;
    u32 offset = (index() % inodes_per_block) * super_block.inode_size;

    auto raw_data = new u8[BLOCK_SIZE(super_block.block_size)];
    MUST(ext2_fs().read_block(block_group_descriptor.inode_table + block(), raw_data));
    memcpy(&m_raw_data, raw_data + offset, sizeof(Ext2RawInode));
    delete[] raw_data;

    m_mode = m_raw_data.mode;
    m_size = m_raw_data.size;

    read_block_pointers();

    if (is_device()) {
        u32 data = m_raw_data.block_pointers[0];
        if (data == 0) {
            data = m_raw_data.block_pointers[1];
        }
        m_major_device_number = (data & 0xFFF00) >> 8;
        m_minor_device_number = (data & 0xFFu) | ((data >> 12u) & 0xFFF00u);
    }
}

u32 Ext2Inode::block_group()
{
    return (m_id - 1) / ext2_fs().super_block().inodes_per_group;
}

u32 Ext2Inode::index()
{
    return (m_id - 1) % ext2_fs().super_block().inodes_per_group;
}

u32 Ext2Inode::block()
{
    return (index() * ext2_fs().super_block().inode_size) / ext2_fs().block_size();
}

u32 Ext2Inode::number_of_blocks() const
{
    return (m_raw_data.size + ext2_fs().block_size() - 1) / ext2_fs().block_size();
}

void Ext2Inode::read_single_block_pointer(u32 single_block_pointer, u32& block_index)
{
    if (block_index >= number_of_blocks()) {
        return;
    }

    u8 buffer[ext2_fs().block_size()];
    ext2_fs().read_blocks(single_block_pointer, 1, buffer);
    for (u32 i = 0; i < ext2_fs().block_pointers_per_block() && block_index < number_of_blocks(); i++) {
        m_block_pointers.add_last(((u32*)buffer)[i]);
        block_index++;
    }
}

void Ext2Inode::read_double_block_pointer(u32 double_block_pointer, u32& block_index)
{
    if (block_index >= number_of_blocks()) {
        return;
    }

    u8 buffer[ext2_fs().block_size()];
    ext2_fs().read_blocks(double_block_pointer, 1, buffer);
    for (u32 i = 0; i < ext2_fs().block_pointers_per_block() && block_index < number_of_blocks(); i++) {
        read_single_block_pointer(((u32*)buffer)[i], block_index);
    }
}

void Ext2Inode::read_triple_block_pointer(u32 triple_block_pointer, u32& block_index)
{
    if (block_index >= number_of_blocks()) {
        return;
    }

    u8 buffer[ext2_fs().block_size()];
    ext2_fs().read_blocks(triple_block_pointer, 1, buffer);
    for (u32 i = 0; i < ext2_fs().block_pointers_per_block() && block_index < number_of_blocks(); i++) {
        read_double_block_pointer(((u32*)buffer)[i], block_index);
    }
}

void Ext2Inode::read_block_pointers()
{
    u32 block_index = 0;
    while (block_index < 12 && block_index < number_of_blocks()) {
        m_block_pointers.add_last(m_raw_data.block_pointers[block_index++]);
    }

    read_single_block_pointer(m_raw_data.s_pointer, block_index);
    read_double_block_pointer(m_raw_data.d_pointer, block_index);
    read_triple_block_pointer(m_raw_data.t_pointer, block_index);
}

u32 Ext2Inode::get_block_pointer(u32 index) const
{
    if (index > m_block_pointers.size()) {
        return 0;
    }
    return m_block_pointers[index];
}

ResultAnd<InodeId> Ext2Inode::find(const String& name)
{
    dbgprintf_if(DEBUG_INODE, "Ext2Inode", "Searching for '%s' in %u blocks in inode %u\n", name.data(), number_of_blocks(), m_id);

    if (!is_directory()) {
        return Result(-ENOTDIR);
    }

    u8 block_buffer[ext2_fs().block_size()];
    char name_buffer[EXT2_NAME_LEN];

    for (u32 i = 0; i < number_of_blocks(); i++) {
        u32 block = get_block_pointer(i);
        ext2_fs().read_blocks(block, 1, block_buffer);

        Ext2RawDirectory* directory = reinterpret_cast<Ext2RawDirectory*>(block_buffer);
        u32 amount_iterated = 0;

        while (directory->inode != 0 && amount_iterated < ext2_fs().block_size()) {
            String directory_name(directory->name, directory->name_length);
            dbgprintf_if(DEBUG_INODE, "Ext2Inode", "name='%s' id=%u\n", directory_name.data(), directory->inode);
            if (name == directory_name) {
                return InodeId(ext2_fs().id(), directory->inode);
            }

            amount_iterated += directory->size;
            directory = reinterpret_cast<Ext2RawDirectory*>((size_t)directory + directory->size);
        }
    }

    return Result(-ENOENT);
}

ResultAnd<ssize_t> Ext2Inode::read(size_t start, size_t length, u8* buffer, FileDescriptor&)
{
    if (length == 0 || m_raw_data.size == 0 || start > m_raw_data.size) {
        return Status::Failure;
    }

    if (start + length > m_raw_data.size) {
        length = m_raw_data.size - start;
    }

    size_t first_block = start / ext2_fs().block_size();
    size_t first_block_start = start % ext2_fs().block_size();
    size_t bytes_left = min(length, (size_t)m_raw_data.size - start);
    size_t block_index = first_block;

    u8 read_buffer[ext2_fs().block_size()];
    while (bytes_left > 0) {
        auto result = ext2_fs().read_blocks(get_block_pointer(block_index), 1, read_buffer);
        if (result.is_error()) {
            return result;
        }

        if (block_index == first_block) {
            if (bytes_left < ext2_fs().block_size() - first_block_start) {
                memcpy(buffer, read_buffer + first_block_start, bytes_left);
                bytes_left = 0;
            } else {
                memcpy(buffer, read_buffer + first_block_start, ext2_fs().block_size() - first_block_start);
                bytes_left -= ext2_fs().block_size() - first_block_start;
                buffer += ext2_fs().block_size() - first_block_start;
            }
        } else {
            if (bytes_left < ext2_fs().block_size()) {
                memcpy(buffer, read_buffer, bytes_left);
                bytes_left = 0;
            } else {
                memcpy(buffer, read_buffer, ext2_fs().block_size());
                bytes_left -= ext2_fs().block_size();
                buffer += ext2_fs().block_size();
            }
        }

        block_index++;
    }

    return length;
}

ResultAnd<ssize_t> Ext2Inode::write(size_t start, size_t length, u8* buffer, FileDescriptor&)
{
    return 0;
}

ResultAnd<int> Ext2Inode::fstat(stat& statbuf, FileDescriptor&)
{
    statbuf.st_dev = (m_major_device_number << 8) + m_minor_device_number;
    statbuf.st_ino = m_id;
    statbuf.st_mode = m_mode;
    statbuf.st_nlink = m_raw_data.hard_links;
    statbuf.st_uid = m_uid;
    statbuf.st_gid = m_gid;
    statbuf.st_size = m_size;
    statbuf.st_blksize = ext2_fs().block_size();
    statbuf.st_blocks = number_of_blocks();

    return 0;
}

void Ext2Inode::open(FileDescriptor&, int flags)
{
}

void Ext2Inode::close(FileDescriptor&)
{
}
