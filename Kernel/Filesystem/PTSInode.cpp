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

ResultAnd<ssize_t> PTSInode::read(FileDescriptor&, size_t start, size_t length, u8* buffer)
{
    return 0;
}

ResultAnd<ssize_t> PTSInode::write(FileDescriptor&, size_t start, size_t length, u8* buffer)
{
    return 0;
}

ResultAnd<ssize_t> PTSInode::get_dir_entries(FileDescriptor&, u8* buffer, ssize_t count)
{
    return 0;
}

ResultAnd<int> PTSInode::fstat(FileDescriptor&, stat&)
{
    return 0;
}

void PTSInode::open(FileDescriptor&, int flags)
{
}

void PTSInode::close(FileDescriptor&)
{
}
