/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/Types.h>

#include <stdio.h>

namespace Universal {

class ShareCountedImpl {
public:
    [[gnu::always_inline]] inline void ref() { m_ref_count++; }

    [[gnu::always_inline]] inline u32 ref_count() const { return m_ref_count; }

private:
    ShareCountedImpl(const ShareCountedImpl&) = delete;
    ShareCountedImpl& operator=(const ShareCountedImpl&) = delete;

    ShareCountedImpl(ShareCountedImpl&&) = delete;
    ShareCountedImpl& operator=(ShareCountedImpl&&) = delete;

protected:
    ShareCountedImpl() { }
    [[gnu::always_inline]] inline ~ShareCountedImpl() { ASSERT(m_ref_count == 0); }

    [[gnu::always_inline]] inline u32 deref_base()
    {
        auto ref_count = m_ref_count--;
        ASSERT(ref_count > 0);
        return ref_count - 1;
    }

    u32 m_ref_count { 1 };
};

template<typename T>
class ShareCounted : public ShareCountedImpl {
public:
    void unref()
    {
        printf("HERE\n");
        u32 ref_count = deref_base();
        if (ref_count == 0) {
            delete static_cast<const T*>(this);
        }
    }
};

}

using Universal::ShareCounted;
