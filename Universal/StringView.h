/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Assert.h>
#include <Universal/Stdlib.h>
#include <Universal/Types.h>

namespace Universal {

class StringView {
public:
    StringView() = default;

    StringView(const char* str)
        : m_str(str)
        , m_length(str == nullptr ? 0 : strlen(str))
    {
    }

    StringView(const char* str, size_t length)
        : m_str(str)
        , m_length(length)
    {
    }

    StringView(const StringView&) = default;

    StringView(StringView&& other)
        : m_str(other.m_str)
        , m_length(other.m_length)
    {
        other.m_str = nullptr;
        other.m_length = 0;
    }

    StringView& operator=(const StringView&) = default;

    StringView& operator=(StringView&& other)
    {
        if (this != &other) {
            m_str = other.m_str;
            m_length = other.m_length;
            other.m_str = nullptr;
            other.m_length = 0;
        }
        return *this;
    }

    constexpr const char* str() const { return m_str; }

    constexpr size_t length() const { return m_length; }

    constexpr bool is_empty() const { return m_length == 0; }
    constexpr bool is_null() const { return m_str == nullptr; }

    constexpr char get(size_t index) const
    {
        ASSERT(index >= 0 || index < m_length);
        return m_str[index];
    }
    constexpr char operator[](size_t index) const { return get(index); }

    bool operator==(const StringView& other) const
    {
        if (is_null()) {
            if (other.is_null()) {
                return true;
            } else {
                return false;
            }
        }

        if (m_length != other.m_length) {
            return false;
        }
        return strncmp(m_str, other.m_str, m_length) == 0;
    }
    bool operator!=(const StringView& other) const { return !(*this == other); }

private:
    constexpr bool check_index(u32 index) const { return index < 0 || index >= m_length; }

    const char* m_str { nullptr };
    size_t m_length { 0 };
};

}

[[nodiscard]] [[gnu::always_inline]] inline Universal::StringView operator""sv(const char* str, size_t length)
{
    return Universal::StringView(str, length);
}

using Universal::StringView;
