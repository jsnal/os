/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/VFS.h>
#include <Universal/Types.h>

class InodeId;

class Filesystem {
public:
    Filesystem()
        : m_filesystem_id(VFS::the().get_next_filesystem_id())
    {
    }

    u32 id() const { return m_filesystem_id; }

    virtual Result init() = 0;
    virtual InodeId root_inode() const = 0;
    virtual SharedPtr<Inode> inode(const InodeId&) = 0;

private:
    u32 m_filesystem_id { 0 };
};
