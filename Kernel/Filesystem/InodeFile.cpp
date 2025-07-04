/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/FileDescriptor.h>
#include <Kernel/Filesystem/Inode.h>
#include <Kernel/Filesystem/InodeFile.h>

InodeFile::InodeFile(SharedPtr<Inode>&& inode)
    : m_inode(move(inode))
{
}

InodeFile::~InodeFile()
{
}

ResultAnd<SharedPtr<FileDescriptor>> InodeFile::open(int flags)
{
    return make_shared_ptr<FileDescriptor>(*this);
}

void InodeFile::close()
{
}

ssize_t InodeFile::read(FileDescriptor& fd, u8* buffer, off_t offset, ssize_t count)
{
    auto result = m_inode->read(offset, count, buffer, fd);
    if (result.is_error()) {
        return -1;
    }

    return result.value();
}

ssize_t InodeFile::write(FileDescriptor&, const u8* buffer, ssize_t count)
{
    return 0;
}

int InodeFile::fstat(FileDescriptor& fd, stat& statbuf)
{
    auto result = m_inode->fstat(statbuf, fd);
    if (result.is_error()) {
        return -1;
    }

    return result.value();
}

int InodeFile::ioctl(FileDescriptor&, uint32_t request, uint32_t* argp)
{
    return 0;
}

size_t InodeFile::length() const
{
    return m_inode->size();
}
