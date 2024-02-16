/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Types.h>

class PageDirectoryEntry {
public:
    enum Flags {
        Present = 1 << 0,
        ReadWrite = 1 << 1,
        UserSupervisor = 1 << 2,
    };

    void* physical_page_base() { return reinterpret_cast<void*>(m_entry & 0xfffff000); }
    u32 entry() const { return m_entry; }

    bool is_present() const { return m_entry & Present; }
    void set_present(bool set) { set_bit(Present, set); }

    bool is_read_write() const { return m_entry & ReadWrite; }
    void set_read_write(bool set) { set_bit(ReadWrite, set); }

    bool is_user_supervisor() const { return m_entry & UserSupervisor; }
    void set_user_supervisor(bool set) { set_bit(UserSupervisor, set); }

private:
    void set_bit(u32 bit, bool value)
    {
        if (value) {
            m_entry |= bit;
        } else {
            m_entry &= ~bit;
        }
    }

    u32 m_entry;
};

class PageTableEntry {
public:
    enum Flags {
        Present = 1 << 0,
        ReadWrite = 1 << 1,
        UserSupervisor = 1 << 2,
    };

    void* physical_page_base() { return reinterpret_cast<void*>(m_entry & 0xfffff000); }
    u32 entry() const { return m_entry; }

    bool is_present() const { return m_entry & Present; }
    void set_present(bool set) { set_bit(Present, set); }

    bool is_read_write() const { return m_entry & ReadWrite; }
    void set_read_write(bool set) { set_bit(ReadWrite, set); }

    bool is_user_supervisor() const { return m_entry & UserSupervisor; }
    void set_user_supervisor(bool set) { set_bit(UserSupervisor, set); }

private:
    void set_bit(u32 bit, bool value)
    {
        if (value) {
            m_entry |= bit;
        } else {
            m_entry &= ~bit;
        }
    }

    u32 m_entry;
};
