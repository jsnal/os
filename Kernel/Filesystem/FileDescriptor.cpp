/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/FileDescriptor.h>

void FileDescriptor::open()
{
    m_file->open(m_flags);
}

void FileDescriptor::close()
{
    m_file->close();
}

ssize_t FileDescriptor::read(u8* buffer, ssize_t count)
{
    return m_file->read(*this, buffer, count);
}
