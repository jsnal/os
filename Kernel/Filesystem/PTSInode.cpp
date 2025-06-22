/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/InodeId.h>
#include <Kernel/Filesystem/PTSFilesystem.h>
#include <Kernel/Filesystem/PTSInode.h>

PTSInode::PTSInode(PTSFilesystem& fs, ino_t id)
    : Inode(fs, id)
{
}

ResultAnd<InodeId> PTSInode::find(const String&)
{
    return Result(Status::Failure);
}

ResultAnd<ssize_t> PTSInode::read(size_t start, size_t length, u8* buffer, FileDescriptor&)
{
    return 0;
}

ResultAnd<ssize_t> PTSInode::write(size_t start, size_t length, u8* buffer, FileDescriptor&)
{
    return 0;
}

void PTSInode::open(FileDescriptor&, int flags)
{
}

void PTSInode::close(FileDescriptor&)
{
}
