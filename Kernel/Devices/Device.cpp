/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/Device.h>
#include <Kernel/Filesystem/FileDescriptor.h>
#include <Universal/ArrayList.h>

static ArrayList<Device*>* s_devices;

ArrayList<Device*>& Device::devices()
{
    if (s_devices == nullptr) {
        s_devices = new ArrayList<Device*>();
    }
    return *s_devices;
}

Device::Device(u32 major, u32 minor)
    : m_major(major)
    , m_minor(minor)
{
    if (get_device(m_major, m_minor) == nullptr) {
        devices().add_last(this);
    }
}

Device* Device::get_device(u32 major, u32 minor)
{
    for (u32 i = 0; i < devices().size(); i++) {
        if (devices()[i]->m_major == major && devices()[i]->m_minor == minor) {
            return devices()[i];
        }
    }

    return nullptr;
}

ResultReturn<SharedPtr<FileDescriptor>> Device::open(int flags)
{
    return make_shared_ptr<FileDescriptor>(*this);
}

void Device::close()
{
}
