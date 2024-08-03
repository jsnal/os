/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/File.h>
#include <Universal/ShareCounted.h>

class FileDescriptor : public ShareCounted<FileDescriptor> {
public:
    FileDescriptor(SharedPtr<File> file)
        : m_file(file)
    {
    }

    void open();
    void close();

    ssize_t write(const u8*, ssize_t count);
    ssize_t read(u8*, ssize_t count);
    int ioctl(uint32_t request, uint32_t* argp);

    int seek(off_t offset, int whence);

    off_t offset() const { return m_offset; }

private:
    SharedPtr<File> m_file;

    int m_flags { 0 };

    off_t m_offset { 0 };
};
