/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Memory/VMObject.h>
#include <Universal/LinkedList.h>

class VirtualRegion : public LinkedListNode<VirtualRegion> {
public:
    VirtualRegion();
    ~VirtualRegion();

    enum Access {
        Read,
        Write,
        Execute
    };

    const VirtualAddress lower() const { return m_lower; }
    const VirtualAddress upper() const { return m_upper; }

    VirtualRegion* m_next { nullptr };
    VirtualRegion* m_previous { nullptr };

private:
    SharedPtr<VMObject> m_vm_object;

    VirtualAddress m_lower;
    VirtualAddress m_upper;

    Access m_access { Read };
};
