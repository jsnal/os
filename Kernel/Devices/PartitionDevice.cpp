/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/PartitionDevice.h>

PartitionDevice::PartitionDevice(UniquePtr<BlockDevice> parent_device, size_t offset)
    : BlockDevice()
    , m_parent_device(move(parent_device))
    , m_offset(offset)
{
}

Result PartitionDevice::read_blocks(u32 block, u32 count, u8* buffer)
{
    return m_parent_device->read_blocks(block + m_offset, count, buffer);
}

Result PartitionDevice::write_blocks(u32 block, u32 count, const u8* buffer)
{
    return m_parent_device->write_blocks(block + m_offset, count, buffer);
}

size_t PartitionDevice::block_size() const
{
    return m_parent_device->block_size();
}
