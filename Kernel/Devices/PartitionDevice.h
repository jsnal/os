/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/BlockDevice.h>
#include <Universal/UniquePtr.h>

class PartitionDevice : public BlockDevice {
public:
    PartitionDevice(UniquePtr<BlockDevice> parent_device, size_t offset);

    Result read_blocks(u32 block, u32 count, u8* buffer) override;
    Result write_blocks(u32 block, u32 count, const u8* buffer) override;
    size_t block_size() const override;

private:
    UniquePtr<BlockDevice> m_parent_device;
    size_t m_offset;
};
