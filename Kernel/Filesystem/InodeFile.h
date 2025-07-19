/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/File.h>
#include <Universal/SharedPtr.h>

class Inode;

class InodeFile : public File {
public:
    InodeFile(SharedPtr<Inode>&&);

    virtual ~InodeFile();

    bool is_inode() override { return true; };

    ResultAnd<SharedPtr<FileDescriptor>> open(int options) override;
    void close() override;

    ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count) override;
    ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count) override;
    ssize_t get_dir_entries(FileDescriptor&, u8* buffer, ssize_t count) override;
    int fstat(FileDescriptor&, stat&) override;
    int ioctl(FileDescriptor&, uint32_t request, uint32_t* argp) override;

    size_t length() const override;

private:
    SharedPtr<Inode> m_inode;
};
