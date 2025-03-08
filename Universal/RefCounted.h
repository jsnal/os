/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/Types.h>

namespace Universal {

class RefCountedImpl {
public:
    [[gnu::always_inline]] inline void ref() { m_ref_count++; }

    [[gnu::always_inline]] inline u32 ref_count() const { return m_ref_count; }

private:
    RefCountedImpl(const RefCountedImpl&) = delete;
    RefCountedImpl& operator=(const RefCountedImpl&) = delete;

    RefCountedImpl(RefCountedImpl&&) = delete;
    RefCountedImpl& operator=(RefCountedImpl&&) = delete;

protected:
    RefCountedImpl() { }
    [[gnu::always_inline]] inline ~RefCountedImpl() { ASSERT(m_ref_count == 0); }

    [[gnu::always_inline]] inline u32 deref_base()
    {
        auto ref_count = m_ref_count--;
        ASSERT(ref_count > 0);
        return ref_count - 1;
    }

    u32 m_ref_count { 1 };
};

template<typename T>
class RefCounted : public RefCountedImpl {
public:
    void unref()
    {
        u32 ref_count = deref_base();
        if (ref_count == 0) {
            delete static_cast<const T*>(this);
        }
    }
};

}

using Universal::RefCounted;
