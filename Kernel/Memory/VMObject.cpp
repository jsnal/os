/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/VMObject.h>

VMObject::VMObject(size_t size)
    : m_physical_pages(size)
{
    MemoryManager::the().add_vm_object(*this);
}

VMObject::~VMObject()
{
    MemoryManager::the().remove_vm_object(*this);
}
