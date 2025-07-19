/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/Device.h>
#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Devices/PartitionDevice.h>
#include <Kernel/Filesystem/DirectoryEntry.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Kernel/Filesystem/FileDescriptor.h>
#include <Kernel/Filesystem/Inode.h>
#include <Kernel/Filesystem/InodeFile.h>
#include <Kernel/Filesystem/InodeId.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/POSIX.h>
#include <LibC/errno_defines.h>
#include <Universal/Logger.h>

VFS& VFS::the()
{
    static VFS m_the;
    return m_the;
}

VFS::VFS()
{
}

void VFS::init()
{
    auto disk = PATADisk::create(PATADisk::Primary, PATADisk::Master);
    if (disk.ptr() == nullptr) {
        panic("Unable to find Disk!\n");
    }

    u8 master_boot_record[512];
    disk->read_blocks(0, 1, master_boot_record);
    u32 partition_offset = *((u32*)&master_boot_record[0x1C6]);

    auto partition_device = PartitionDevice::create(disk.leak_ptr(), partition_offset);

    m_root_filesystem = make_unique_ptr<Ext2Filesystem>(partition_device.leak_ptr());
    if (m_root_filesystem.ptr() == nullptr) {
        panic("Unable to create root filesystem\n");
    }

    if (root_filesystem().init().is_error()) {
        panic("Unable to initialize root filesystem\n");
    }

    auto root_inode_id = root_filesystem().root_inode();
    auto root_inode = root_filesystem().inode(root_inode_id);

    if (!root_inode->is_directory()) {
        panic("Root filesystem is invalid\n");
    }

    m_root_inode = move(root_inode);

    m_random_device = make_unique_ptr<RandomDevice>();

    dbgprintf("VFS", "VFS initialized\n");
}

ResultAnd<SharedPtr<FileDescriptor>> VFS::open(const String& path, int flags, mode_t mode, DirectoryEntry& base)
{
    auto entry = TRY_TAKE(traverse_path(path, base));
    dbgprintf("VFS", "Found inode %u to open for '%s'\n", entry->inode().id(), path.data());

    if (flags & O_DIRECTORY && !entry->inode().is_directory()) {
        return Result(-ENOTDIR);
    }

    if (entry->inode().is_device()) {
        u32 major = entry->inode().major_device_number();
        u32 minor = entry->inode().minor_device_number();

        auto device = Device::get_device(major, minor);
        dbgprintf("VFS", "We've got a device! %u:%u\n", device->major(), device->minor());

        return device->open(0);
    }

    // TODO: Add many more flags here!

    return FileDescriptor::create(*entry);
}

ResultAnd<SharedPtr<DirectoryEntry>> VFS::open_directory(const String& path, DirectoryEntry& base)
{
    auto entry = TRY_TAKE(traverse_path(path, base));

    if (!entry->inode().is_directory()) {
        return Result(-ENOTDIR);
    }

    return entry;
}

ResultAnd<SharedPtr<DirectoryEntry>> VFS::traverse_path(const String& path, DirectoryEntry& base)
{
    dbgprintf("VFS", "Starting to traverse the path for '%s'\n", path.data());

    SharedPtr<DirectoryEntry> current_entry = path[0] == '/' ? DirectoryEntry::create(nullptr, *m_root_inode) : base;

    auto split_path = path.split('/');
    for (size_t i = 0; i < split_path.size(); i++) {
        if (!current_entry->inode().is_directory()) {
            return Result(-ENOTDIR);
        }

        // TODO: Add support for consecutive '..'
        if (split_path[i] == "..") {
            if (current_entry->has_parent()) {
                current_entry = current_entry->parent();
            }
            continue;
        } else if (split_path[i] == ".") {
            continue;
        }

        InodeId inode_id = TRY_TAKE(current_entry->inode().find(split_path[i]));
        current_entry = DirectoryEntry::create(current_entry.ptr(), *m_root_filesystem->inode(inode_id));
    }

    return current_entry;
}
