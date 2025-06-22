/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Filesystem/InodeId.h>
#include <Kernel/Filesystem/PTSFilesystem.h>
#include <Kernel/Filesystem/PTSInode.h>

PTSFilesystem::PTSFilesystem()
    : Filesystem()
{
}

Result PTSFilesystem::init()
{
    m_root_inode = make_shared_ptr<PTSInode>(*this, 0);
    return Status::OK;
}

InodeId PTSFilesystem::root_inode() const
{
    return InodeId(id(), 0);
}

SharedPtr<Inode> PTSFilesystem::inode(const InodeId& inode_id)
{
    if (inode_id.id() == 0) {
        return m_root_inode;
    }

    auto* device = Device::get_device(136, inode_id.id() - 1);

    auto inode = make_shared_ptr<PTSInode>(*this, inode_id.id());
    m_root_inode->m_major_device_number = device->major();
    m_root_inode->m_minor_device_number = device->minor();
    m_root_inode->m_mode = 0020644;

    return inode;
}
