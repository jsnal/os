/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Inode.h>
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
    // if (index > )
    return 0;
}

Result Inode::read(u8* buffer)
{
    dbgprintf("Inode", "Number of block pointers %d\n", m_block_pointers.size());
    dbgprintf("Inode", "reading: %d\n", m_block_pointers[0]);
    m_fs.read_blocks(m_block_pointers[0], 1, buffer);

    return Result::OK;
}
