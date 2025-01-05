/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Stdlib.h>
#include <Universal/Types.h>

namespace Universal {

template<typename CharT>
class BasicString {
public:
    BasicString()
        : m_size(0)
    {
        m_inline_data[0] = '\0';
    }

    BasicString(const CharT* string)
        : m_size(strlen(string))
    {
        if (!is_inlined()) {
            m_capacity = m_size;
            m_data = new CharT[m_capacity + 1];
        }

        memcpy(data(), string, m_size * sizeof(CharT));
        data()[m_size] = '\0';
    }

    BasicString(const BasicString& other)
        : m_size(other.m_size)
    {
        if (!is_inlined()) {
            m_capacity = other.m_capacity;
            m_data = new CharT[m_capacity + 1];
        }

        memcpy(data(), other.data(), m_size + 1);
    }

    BasicString(BasicString&& other)
        : m_size(other.m_size)
    {
        if (is_inlined()) {
            memcpy(data(), other.data(), m_size + 1);
        } else {
            m_data = other.m_data;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_capacity = 0;
        }

        other.m_size = 0;
        memset(other.m_inline_data, 0, kInlineCapacity);
    }

    ~BasicString()
    {
        if (!is_inlined()) {
            delete[] m_data;
        }
    }

    BasicString& operator=(const BasicString& other)
    {
        if (this != &other) {
        }
        return *this;
    }

    const char* str() const { return data(); }

    size_t length() const { return m_size; }

private:
    const CharT* data() const { return is_inlined() ? m_inline_data : m_data; }
    CharT* data() { return is_inlined() ? m_inline_data : m_data; }

    constexpr bool is_inlined() const { return m_size <= kInlineCapacity; }

    void ensure_capacity(size_t capacity);

    static constexpr u8 kInlineCapacity = 15 / sizeof(CharT);

    union {
        struct {
            CharT* m_data;
            size_t m_capacity;
        };
        alignas(CharT) CharT m_inline_data[kInlineCapacity + 1] {};
    };

    size_t m_size;
};

using string = BasicString<char>;

}

template class Universal::BasicString<char>;

using Universal::BasicString;
