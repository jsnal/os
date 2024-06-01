/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/API/errno.h>
#include <Kernel/Filesystem/Inode.h>
#include <Universal/Number.h>
#include <Universal/Stdlib.h>

Inode::Inode(Ext2Filesystem& fs, ino_t id)
    : m_fs(fs)
    , m_id(id)
{
    m_block_pointers = ArrayList<u32>();

    auto& super_block = m_fs.super_block();
    auto& block_group_descriptor = m_fs.block_group_descriptor(block_group());

    u32 inodes_per_block = BLOCK_SIZE(super_block.block_size) / super_block.inode_size;
    u32 offset = (index() % inodes_per_block) * super_block.inode_size;
    auto result = m_fs.read_blocks(block_group_descriptor.inode_table + block(), 1);
    if (result.is_ok()) {
        memcpy(&m_raw_data, result.value() + offset, sizeof(Ext2Inode));
        delete result.value();
    }

    read_block_pointers();
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

u32 Inode::number_of_blocks() const
{
    return (m_raw_data.size + m_fs.block_size() - 1) / m_fs.block_size();
}

u32 Inode::major_device_number() const
{
    u32 data = m_raw_data.block_pointers[0];
    if (data == 0) {
        data = m_raw_data.block_pointers[1];
    }

    return (data & 0xFFF00) >> 8;
}

u32 Inode::minor_device_number() const
{
    u32 data = m_raw_data.block_pointers[0];
    if (data == 0) {
        data = m_raw_data.block_pointers[1];
    }

    return (data & 0xFFu) | ((data >> 12u) & 0xFFF00u);
}

ResultReturn<InodeId> Inode::find(const String& name)
{
    dbgprintf_if(DEBUG_INODE, "Inode", "Searching for '%s' in %u blocks in inode %u\n", name.str(), number_of_blocks(), m_id);

    if (!is_directory()) {
        return Result(EISDIR);
    }

    u8 block_buffer[m_fs.block_size()];
    char name_buffer[EXT2_NAME_LEN];

    for (u32 i = 0; i < number_of_blocks(); i++) {
        u32 block = get_block_pointer(i);
        m_fs.read_blocks(block, 1, block_buffer);

        Ext2Directory* directory = reinterpret_cast<Ext2Directory*>(block_buffer);

        u32 amount_iterated = 0;

        while (directory->inode != 0 && amount_iterated < m_fs.block_size()) {
            dbgprintf_if(DEBUG_INODE, "Inode", "name='%s' id=%u\n", directory->name, directory->inode);
            if (name == directory->name) {
                return InodeId(m_fs.id(), directory->inode);
            }

            amount_iterated += directory->size;
            directory = reinterpret_cast<Ext2Directory*>((size_t)directory + directory->size);
        }
    }

    return Result(ENOENT);
}

void Inode::read_single_block_pointer(u32 single_block_pointer, u32& block_index)
{
    if (block_index >= number_of_blocks()) {
        return;
    }

    u8 buffer[m_fs.block_size()];
    m_fs.read_blocks(single_block_pointer, 1, buffer);
    for (u32 i = 0; i < m_fs.block_pointers_per_block() && block_index < number_of_blocks(); i++) {
        m_block_pointers.add_last(((u32*)buffer)[i]);
        block_index++;
    }
}

void Inode::read_double_block_pointer(u32 double_block_pointer, u32& block_index)
{
    if (block_index >= number_of_blocks()) {
        return;
    }

    u8 buffer[m_fs.block_size()];
    m_fs.read_blocks(double_block_pointer, 1, buffer);
    for (u32 i = 0; i < m_fs.block_pointers_per_block() && block_index < number_of_blocks(); i++) {
        read_single_block_pointer(((u32*)buffer)[i], block_index);
    }
}

void Inode::read_triple_block_pointer(u32 triple_block_pointer, u32& block_index)
{
    if (block_index >= number_of_blocks()) {
        return;
    }

    u8 buffer[m_fs.block_size()];
    m_fs.read_blocks(triple_block_pointer, 1, buffer);
    for (u32 i = 0; i < m_fs.block_pointers_per_block() && block_index < number_of_blocks(); i++) {
        read_double_block_pointer(((u32*)buffer)[i], block_index);
    }
}

void Inode::read_block_pointers()
{
    u32 block_index = 0;
    while (block_index < 12 && block_index < number_of_blocks()) {
        m_block_pointers.add_last(m_raw_data.block_pointers[block_index++]);
    }

    read_single_block_pointer(m_raw_data.s_pointer, block_index);
    read_double_block_pointer(m_raw_data.d_pointer, block_index);
    read_triple_block_pointer(m_raw_data.t_pointer, block_index);
}

u32 Inode::get_block_pointer(u32 index) const
{
    if (index > m_block_pointers.size()) {
        return 0;
    }
    return m_block_pointers[index];
}

ResultReturn<ssize_t> Inode::read(size_t start, size_t length, u8* buffer)
{
    if (length == 0 || m_raw_data.size == 0 || start > m_raw_data.size) {
        return Result::Failure;
    }

    if (start + length > m_raw_data.size) {
        length = m_raw_data.size - start;
    }

    size_t first_block = start / m_fs.block_size();
    size_t first_block_start = start % m_fs.block_size();
    size_t bytes_left = min(length, (size_t)m_raw_data.size - start);
    size_t block_index = first_block;

    u8 read_buffer[m_fs.block_size()];
    while (bytes_left > 0) {
        auto result = m_fs.read_blocks(get_block_pointer(block_index), 1, read_buffer);
        if (result.is_error()) {
            return result;
        }

        if (block_index == first_block) {
            if (bytes_left < m_fs.block_size() - first_block_start) {
                memcpy(buffer, read_buffer + start, bytes_left);
                bytes_left = 0;
            } else {
                memcpy(buffer, read_buffer + start, m_fs.block_size() - first_block_start);
                bytes_left -= m_fs.block_size() - first_block_start;
                buffer += m_fs.block_size() - first_block_start;
            }
        } else {
            if (bytes_left < m_fs.block_size()) {
                memcpy(buffer, read_buffer, bytes_left);
                bytes_left = 0;
            } else {
                memcpy(buffer, read_buffer, m_fs.block_size());
                bytes_left -= m_fs.block_size();
                buffer += m_fs.block_size();
            }
        }

        block_index++;
    }

    return length;
}
