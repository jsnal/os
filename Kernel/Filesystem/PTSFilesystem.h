/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/Filesystem.h>

class PTSInode;

class PTSFilesystem : public Filesystem {
public:
    PTSFilesystem();

    virtual Result init() override;
    virtual InodeId root_inode() const override;
    virtual SharedPtr<Inode> inode(const InodeId&) override;

private:
    SharedPtr<PTSInode> m_root_inode;
};
