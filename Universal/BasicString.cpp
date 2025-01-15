/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/BasicString.h>
#include <Universal/Malloc.h>

template<typename CharT>
void BasicString<CharT>::ensure_capacity(size_t capacity)
{
    if (is_inlined()) {
        if (capacity <= kInlineCapacity) {
            return;
        }

        // Ran out of room in the inline buffer. Switch to the heap
        m_capacity = m_capacity + (m_capacity / 2);
        m_data = new CharT[m_capacity + 1];
        memcpy(m_data, m_inline_data, m_size * sizeof(CharT));
        m_data[m_size] = '\0';
    } else if (capacity > m_capacity) {
        m_capacity = m_capacity + (m_capacity / 2);
        CharT* new_data = new CharT[m_capacity + 1];
        memcpy(new_data, m_data, m_size * sizeof(CharT));
        delete[] m_data;
        m_data = new_data;
    }
}
