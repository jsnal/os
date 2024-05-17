/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Types.h>

class InodeId final {
public:
    InodeId()
    {
    }

    InodeId(u32 filesystem_id, u32 inode)
        : m_filesystem_id(filesystem_id)
        , m_inode(inode)
    {
    }

    ino_t id() const { return m_inode; }
    u32 filesystem_id() const { return m_filesystem_id; }

private:
    u32 m_filesystem_id;
    ino_t m_inode;
};
