/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/FileDescriptor.h>
#include <Kernel/Filesystem/InodeFile.h>
#include <Kernel/Logger.h>
#include <Kernel/POSIX.h>
#include <LibC/errno_defines.h>

void FileDescriptor::open()
{
    m_file->open(m_flags);
}

void FileDescriptor::close()
{
    m_file->close();
}

ssize_t FileDescriptor::write(const u8* buffer, ssize_t count)
{
    return m_file->write(*this, buffer, count);
}

ssize_t FileDescriptor::read(u8* buffer, ssize_t count)
{
    return m_file->read(*this, buffer, m_offset, count);
}

int FileDescriptor::ioctl(uint32_t request, uint32_t* argp)
{
    return m_file->ioctl(*this, request, argp);
}

int FileDescriptor::seek(off_t offset, int whence)
{
    off_t new_offset = m_offset;

    switch (whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset += offset;
            break;
        case SEEK_END:
            if (!m_file->is_inode()) {
                return -EIO;
            }
            new_offset = m_file->length() + offset;
            break;
        default:
            return -EINVAL;
    }

    if (new_offset < 0 || (m_file->is_inode() && new_offset > m_file->length())) {
        return -EINVAL;
    }

    m_offset = new_offset;
    return m_offset;
}
