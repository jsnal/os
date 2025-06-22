/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/File.h>
#include <Universal/ArrayList.h>

class Device : public File {
public:
    static Device* get_device(u32 major, u32 minor);

    virtual ResultAnd<SharedPtr<FileDescriptor>> open(int flags) override;
    virtual void close() override;

    u32 major() const { return m_major; }
    u32 minor() const { return m_minor; }

protected:
    Device(u32 major, u32 minor);

    static ArrayList<Device*>& devices();

private:
    u32 m_major { 0 };
    u32 m_minor { 0 };
};
