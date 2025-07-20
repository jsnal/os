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
    static SharedPtr<DirectoryEntry> create(DirectoryEntry* parent, Inode& inode, const String& name)
    {
        return adopt_shared_ptr(*new DirectoryEntry(parent, inode, name));
    }

    DirectoryEntry& parent() { return *m_parent; }
    const DirectoryEntry& parent() const { return *m_parent; }

    Inode& inode() { return *m_inode; }
    const Inode& inode() const { return *m_inode; }

    const String& name() const { return m_name; }

    bool has_parent() const { return !m_parent.is_null(); }

    String absolute_path() const
    {
        if (!has_parent()) {
            return "/";
        }

        const DirectoryEntry* entries[32];
        const DirectoryEntry* entry = this;
        ssize_t entry_count = 0;
        do {
            entries[entry_count++] = entry;
            entry = &entry->parent();
        } while (entry->has_parent());

        String path;
        for (entry_count--; entry_count >= 0; entry_count--) {
            path += "/";
            path += entries[entry_count]->name();
        }
        return path;
    }

private:
    DirectoryEntry(DirectoryEntry* parent, Inode& inode, const String& name)
        : m_parent(parent)
        , m_inode(inode)
        , m_name(name)
    {
    }

    SharedPtr<DirectoryEntry> m_parent;
    SharedPtr<Inode> m_inode;
    String m_name;
};
