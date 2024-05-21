/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/FileDescriptor.h>
#include <Kernel/Filesystem/Inode.h>
#include <Kernel/Filesystem/InodeFile.h>

InodeFile::InodeFile(SharedPtr<Inode> inode)
    : m_inode(inode)
{
}

InodeFile::~InodeFile()
{
}

ResultOr<SharedPtr<FileDescriptor>> InodeFile::open(int flags)
{
    return adopt(*new FileDescriptor(*this));
}

void InodeFile::close()
{
}

ssize_t InodeFile::read(FileDescriptor& fd, u8* buffer, ssize_t count)
{
    auto result = m_inode->read(fd.offset(), count, buffer);
    if (result.is_error()) {
        return -1;
    }

    return result.value();
}

ssize_t InodeFile::write(FileDescriptor&, const u8* buffer, ssize_t count)
{
    return 0;
}
