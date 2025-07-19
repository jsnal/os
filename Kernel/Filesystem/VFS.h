/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/RandomDevice.h>
#include <Universal/BasicString.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/UniquePtr.h>

class DirectoryEntry;
class Filesystem;
class FileDescriptor;
class Inode;

class VFS final {
public:
    static VFS& the();

    VFS();

    void init();

    ResultAnd<SharedPtr<FileDescriptor>> open(const String& path, int flags, mode_t mode, DirectoryEntry& base);
    ResultAnd<SharedPtr<DirectoryEntry>> open_directory(const String& path, DirectoryEntry& base);

    u32 get_next_filesystem_id() { return m_current_filesystem_id++; }

    Filesystem& root_filesystem() { return *m_root_filesystem; }
    Inode& root_inode() { return *m_root_inode; }

private:
    ResultAnd<SharedPtr<DirectoryEntry>> traverse_path(const String& path, DirectoryEntry& base);

    u32 m_current_filesystem_id { 0 };

    UniquePtr<RandomDevice> m_random_device;

    UniquePtr<Filesystem> m_root_filesystem;
    SharedPtr<Inode> m_root_inode;
};
