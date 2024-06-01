/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/RandomDevice.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/String.h>
#include <Universal/UniquePtr.h>

class Inode;
class Ext2Filesystem;
class FileDescriptor;

class VFS final {
public:
    static VFS& the();

    VFS();

    void init();

    ResultReturn<SharedPtr<FileDescriptor>> open(const String& path, int flags, mode_t mode);

    u32 get_next_filesystem_id() { return m_current_filesystem_id++; }

    Ext2Filesystem& root_filesystem() { return *m_root_filesystem; }
    Inode& root_inode() { return *m_root_inode; }

private:
    ResultReturn<SharedPtr<Inode>> traverse_path(const String& path, SharedPtr<Inode>& base);

    u32 m_current_filesystem_id { 0 };

    UniquePtr<RandomDevice> m_random_device;

    UniquePtr<Ext2Filesystem> m_root_filesystem;
    SharedPtr<Inode> m_root_inode;
};
