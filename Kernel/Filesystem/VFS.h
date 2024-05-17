/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Inode.h>
#include <Universal/Result.h>

class Inode;

class VFS final {
public:
    static VFS& the();

    VFS();

    void init();

    u32 get_next_filesystem_id() { return m_current_filesystem_id++; }

private:
    u32 m_current_filesystem_id { 0 };

    SharedPtr<Inode> m_root_inode;
};
