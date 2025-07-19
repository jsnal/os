/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Inode.h>
#include <Universal/RefCounted.h>
#include <Universal/SharedPtr.h>

class DirectoryEntry : public RefCounted<DirectoryEntry> {
public:
    static SharedPtr<DirectoryEntry> create(DirectoryEntry* parent, Inode& inode)
    {
        return adopt_shared_ptr(*new DirectoryEntry(parent, inode));
    }

    DirectoryEntry& parent() { return *m_parent; }
    const DirectoryEntry& parent() const { return *m_parent; }

    Inode& inode() { return *m_inode; }
    const Inode& inode() const { return *m_inode; }

    bool has_parent() const { return !m_parent.is_null(); }

private:
    DirectoryEntry(DirectoryEntry* parent, Inode& inode)
        : m_parent(parent)
        , m_inode(inode)
    {
    }

    SharedPtr<DirectoryEntry> m_parent;
    SharedPtr<Inode> m_inode;
};
