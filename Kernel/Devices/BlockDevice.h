/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Result.h>
#include <Universal/Types.h>

class BlockDevice {
public:
    BlockDevice()
    {
    }

    Result read_block(u32 block, u8* buffer)
    {
        return read_blocks(block, 1, buffer);
    }

    Result write_block(u32 block, const u8* buffer)
    {
        return write_blocks(block, 1, buffer);
    }

    virtual Result read_blocks(u32 block, u32 count, u8* buffer) = 0;
    virtual Result write_blocks(u32 block, u32 count, const u8* buffer) = 0;
    virtual size_t block_size() const = 0;

    // TODO: Eventually this will override a is_block_device() in a parent
    //       class called Device.
    bool is_block_device() { return true; };
};
