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

    virtual ResultAnd<InodeId> find(const String&) override;
    virtual ResultAnd<ssize_t> read(size_t start, size_t length, u8* buffer, FileDescriptor&) override;
    virtual ResultAnd<ssize_t> write(size_t start, size_t length, u8* buffer, FileDescriptor&) override;
    virtual void open(FileDescriptor&, int flags) override;
    virtual void close(FileDescriptor&) override;
};
