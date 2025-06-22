/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Filesystem.h>
#include <Universal/ArrayList.h>
#include <Universal/BasicString.h>
#include <Universal/RefCounted.h>
#include <Universal/Types.h>

class FileDescriptor;
class InodeId;

#define MODE_CHARACTER_DEVICE 0x2000
#define MODE_DIRECTORY 0x4000
#define MODE_BLOCK_DEVICE 0x6000

class Inode : public RefCounted<Inode> {
public:
    Inode(Filesystem& fs, ino_t id)
        : m_fs(fs)
        , m_id(id)
    {
    }

    virtual ~Inode() = default;

    bool is_directory() const { return (m_mode & 0xF000) == MODE_DIRECTORY; };
    bool is_character_device() const { return (m_mode & 0xF000) == MODE_CHARACTER_DEVICE; };
    bool is_block_device() const { return (m_mode & 0xF000) == MODE_BLOCK_DEVICE; };
    bool is_device() const { return is_block_device() || is_character_device(); }

    virtual ResultAnd<InodeId> find(const String&) = 0;
    virtual ResultAnd<ssize_t> read(size_t start, size_t length, u8* buffer, FileDescriptor&) = 0;
    virtual ResultAnd<ssize_t> write(size_t start, size_t length, u8* buffer, FileDescriptor&) = 0;
    virtual void open(FileDescriptor&, int flags) = 0;
    virtual void close(FileDescriptor&) = 0;
    virtual bool can_read(FileDescriptor&) { return true; };
    virtual bool can_write(FileDescriptor&) { return true; };

    Filesystem& fs() { return m_fs; }
    const Filesystem& fs() const { return m_fs; }

    ino_t id() const { return m_id; }
    uid_t uid() const { return m_uid; }
    gid_t gid() const { return m_gid; }
    u16 mode() const { return m_mode; }
    u32 size() const { return m_size; }
    u32 major_device_number() const { return m_major_device_number; }
    u32 minor_device_number() const { return m_minor_device_number; }

protected:
    ino_t m_id { 0 };
    uid_t m_uid { 0 };
    gid_t m_gid { 0 };
    u16 m_mode { 0 };
    u32 m_size { 0 };
    u32 m_major_device_number { 0 };
    u32 m_minor_device_number { 0 };

private:
    Filesystem& m_fs;
};
