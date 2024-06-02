/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/Inode.h>

class PTSFilesystem;

class PTSInode : public Inode {
    friend PTSFilesystem;

public:
    PTSInode(PTSFilesystem& fs, ino_t id);

    virtual ResultReturn<InodeId> find(const String&) override;
    virtual ResultReturn<ssize_t> read(size_t start, size_t length, u8* buffer, FileDescriptor&) override;
    virtual ResultReturn<ssize_t> write(size_t start, size_t length, u8* buffer, FileDescriptor&) override;
    virtual void open(FileDescriptor&, int flags) override;
    virtual void close(FileDescriptor&) override;
};
