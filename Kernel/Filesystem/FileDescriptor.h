/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/DirectoryEntry.h>
#include <Kernel/Filesystem/File.h>
#include <Universal/RefCounted.h>

class FileDescriptor : public RefCounted<FileDescriptor> {
public:
    static SharedPtr<FileDescriptor> create(DirectoryEntry& entry);
    static SharedPtr<FileDescriptor> create(File& file);

    void open();
    void close();
    ssize_t write(const u8*, ssize_t count);
    ssize_t read(u8*, ssize_t count);
    ssize_t get_dir_entries(u8*, ssize_t count);
    int fstat(stat&);
    int ioctl(uint32_t request, uint32_t* argp);
    int seek(off_t offset, int whence);

    int flags() const { return m_flags; }
    off_t offset() const { return m_offset; }

    const File& file() const { return *m_file; }
    File& file() { return *m_file; }

    const DirectoryEntry* directory_entry() const { return m_directory_entry.ptr(); }
    DirectoryEntry* directory_entry() { return m_directory_entry.ptr(); }

private:
    FileDescriptor(File& file)
        : m_file(file)
    {
    }

    SharedPtr<File> m_file;
    SharedPtr<DirectoryEntry> m_directory_entry;
    int m_flags { 0 };
    off_t m_offset { 0 };
};
