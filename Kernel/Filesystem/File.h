/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Result.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>

class FileDescriptor;

class File : public ShareCounted<File> {
public:
    File() = default;
    virtual ~File() = default;

    virtual bool is_inode() { return false; };
    virtual bool is_character_device() { return false; };
    virtual bool is_block_device() { return false; };

    virtual size_t length() const { return 0; }

    virtual ResultReturn<SharedPtr<FileDescriptor>> open(int flags) = 0;
    virtual void close() = 0;

    virtual ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count) = 0;
    virtual ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count) = 0;
    virtual int ioctl(FileDescriptor&, uint32_t request, uint32_t* argp) = 0;
};
