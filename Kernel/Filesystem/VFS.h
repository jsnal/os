/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

// #include <Kernel/Filesystem/Inode.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/String.h>
#include <Universal/UniquePtr.h>

class Inode;
class Ext2Filesystem;

class VFS final {
public:
    static VFS& the();

    VFS();

    void init();

    ResultOr<u32> open(const String& path, mode_t mode);

    u32 get_next_filesystem_id() { return m_current_filesystem_id++; }

    Ext2Filesystem& root_filesystem() { return *m_root_filesystem; }
    Inode& root_inode() { return *m_root_inode; }

private:
    ResultOr<SharedPtr<Inode>> traverse_path(const String& path, SharedPtr<Inode>& base);

    u32 m_current_filesystem_id { 0 };

    UniquePtr<Ext2Filesystem> m_root_filesystem;
    SharedPtr<Inode> m_root_inode;
};
