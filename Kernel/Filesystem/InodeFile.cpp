/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/FileDescriptor.h>
#include <Kernel/Filesystem/Inode.h>
#include <Kernel/Filesystem/InodeFile.h>

SharedPtr<InodeFile> InodeFile::create(SharedPtr<Inode>&& inode)
{
    return adopt_shared_ptr(*new InodeFile(move(inode)));
}

InodeFile::~InodeFile()
{
}

ResultAnd<SharedPtr<FileDescriptor>> InodeFile::open(int flags)
{
    return FileDescriptor::create(*this);
}

void InodeFile::close()
{
}

ssize_t InodeFile::read(FileDescriptor& fd, u8* buffer, off_t offset, ssize_t count)
{
    return m_inode->read(fd, offset, count, buffer);
}

ssize_t InodeFile::write(FileDescriptor&, const u8* buffer, ssize_t count)
{
    return 0;
}

ssize_t InodeFile::get_dir_entries(FileDescriptor& fd, u8* buffer, ssize_t count)
{
    return m_inode->get_dir_entries(fd, buffer, count);
}

int InodeFile::fstat(FileDescriptor& fd, stat& statbuf)
{
    return m_inode->fstat(fd, statbuf);
}

int InodeFile::ioctl(FileDescriptor&, uint32_t request, uint32_t* argp)
{
    return 0;
}

size_t InodeFile::length() const
{
    return m_inode->size();
}
