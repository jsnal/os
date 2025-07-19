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

    ResultAnd<InodeId> find(const String&) override;
    ResultAnd<ssize_t> read(FileDescriptor&, size_t start, size_t length, u8* buffer) override;
    ResultAnd<ssize_t> write(FileDescriptor&, size_t start, size_t length, u8* buffer) override;
    ResultAnd<ssize_t> get_dir_entries(FileDescriptor&, u8* buffer, ssize_t count) override;
    ResultAnd<int> fstat(FileDescriptor&, stat&) override;
    void open(FileDescriptor&, int flags) override;
    void close(FileDescriptor&) override;
};
