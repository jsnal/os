/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/Address.h>
#include <Universal/Array.h>
#include <Universal/LinkedList.h>
#include <Universal/RefCounted.h>
#include <Universal/SharedPtr.h>

class VMObject : public RefCounted<VMObject>
    , public LinkedListNode<VMObject> {
public:
    VMObject(size_t);
    ~VMObject();

    VMObject* m_next { nullptr };
    VMObject* m_previous { nullptr };

private:
    Array<PhysicalAddress> m_physical_pages;
};
