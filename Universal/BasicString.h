/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/ArrayList.h>
#include <Universal/Stdlib.h>
#include <Universal/StringView.h>
#include <Universal/Types.h>

namespace Universal {

template<typename CharT>
class BasicString {
public:
    BasicString()
        : m_length(0)
    {
        m_inline_data[0] = '\0';
    }

    BasicString(const CharT* str)
        : m_capacity(str == nullptr ? 0 : strlen(str))
        , m_length(m_capacity)
    {
        m_length = m_capacity;
        if (!is_inlined()) {
            m_data = new CharT[m_capacity + 1];
        }

        memcpy(data(), str, m_length * sizeof(CharT));
        data()[m_length] = '\0';
    }

    BasicString(const StringView& sv)
        : m_capacity(sv.length())
        , m_length(m_capacity)
    {
        m_length = m_capacity;
        if (!is_inlined()) {
            m_data = new CharT[m_capacity + 1];
        }

        memcpy(data(), sv.str(), m_length * sizeof(CharT));
        data()[m_length] = '\0';
    }

    BasicString(const BasicString& other)
        : m_capacity(other.m_capacity)
        , m_length(other.m_length)
    {
        if (!is_inlined()) {
            m_data = new CharT[m_capacity + 1];
        }

        memcpy(data(), other.data(), m_length + 1);
    }

    BasicString(BasicString&& other)
        : m_capacity(other.m_capacity)
        , m_length(other.m_length)
    {
        if (is_inlined()) {
            memcpy(data(), other.data(), m_length + 1);
        } else {
            m_data = other.m_data;
            other.m_data = nullptr;
        }

        other.m_length = 0;
        other.m_capacity = 0;
        memset(other.m_inline_data, 0, kInlineCapacity);
    }

    ~BasicString() { clear(); }

    static BasicString format(const char* format, ...);

    BasicString& operator=(const BasicString& other)
    {
        if (this != &other) {
            m_length = other.m_length;
            m_capacity = other.m_capacity;
            if (!is_inlined()) {
                m_data = new CharT[m_capacity + 1];
            }

            memcpy(data(), other.data(), m_length + 1);
        }
        return *this;
    }

    BasicString& operator=(BasicString&& other)
    {
        if (this != &other) {
            clear();

            m_length = other.m_length;
            m_capacity = other.m_capacity;

            if (is_inlined()) {
                memcpy(data(), other.data(), m_length + 1);
            } else {
                m_data = other.m_data;
                other.m_data = nullptr;
            }

            other.m_length = 0;
            other.m_capacity = 0;
            memset(other.m_inline_data, 0, kInlineCapacity);
        }
        return *this;
    }

    StringView string_view() const { return data(); }

    char operator[](size_t index) const { return data()[index]; }

    bool operator==(const BasicString&) const;
    bool operator!=(const BasicString& other) const { return !(*this == other); };

    bool operator==(const CharT*) const;
    bool operator!=(const CharT* other) const { return !(*this == other); };

    void concat(const BasicString& other) { concat(other.data(), other.length()); }
    void concat(const StringView& other) { concat(other.str(), other.length()); }
    void concat(const CharT* other)
    {
        if (other != nullptr) {
            concat(other, strlen(other));
        }
    }

    BasicString& operator+=(const BasicString& other)
    {
        concat(other.data(), other.length());
        return *this;
    }
    BasicString& operator+=(const StringView& other)
    {
        concat(other.str(), other.length());
        return *this;
    }
    BasicString& operator+=(const CharT* other)
    {
        if (other != nullptr) {
            concat(other, strlen(other));
        }
        return *this;
    }

    BasicString operator+(const BasicString& other)
    {
        BasicString copy(*this);
        copy.concat(other.data(), other.length());
        return copy;
    }
    BasicString operator+(const StringView& other)
    {
        BasicString copy(*this);
        copy.concat(other.str(), other.length());
        return copy;
    }
    BasicString operator+(const CharT* other)
    {
        BasicString copy(*this);
        if (other != nullptr) {
            copy.concat(other, strlen(other));
        }
        return copy;
    }

    BasicString substring(size_t start, size_t end) const;
    ArrayList<BasicString> split(char delimiter = ' ') const;

    bool contains(const BasicString& str) const { return string_view().contains(str.string_view()); };
    bool contains(const StringView& sv) const { return string_view().contains(sv); };
    bool contains(const char* str) const { return string_view().contains(str); };

    Optional<size_t> find(char needle) const { return string_view().find(needle); };
    Optional<size_t> find_last(char needle) const { return string_view().find_last(needle); };

    const CharT* data() const { return is_inlined() ? m_inline_data : m_data; }
    CharT* data() { return is_inlined() ? m_inline_data : m_data; }

    bool is_empty() const { return m_length == 0; }
    size_t length() const { return m_length; }

private:
    constexpr bool is_inlined() const { return m_capacity <= kInlineCapacity; }

    void concat(const CharT*, size_t);
    void ensure_capacity(size_t capacity);

    void clear()
    {
        if (!is_inlined()) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_length = 0;
        m_capacity = 0;
    }

    static constexpr u8 kInlineCapacity = 15 / sizeof(CharT);

    CharT* m_data { nullptr };
    alignas(CharT) CharT m_inline_data[kInlineCapacity + 1] {};
    size_t m_capacity { 0 };
    size_t m_length { 0 };
};

}

template class Universal::BasicString<char>;

using String = class Universal::BasicString<char>;
using Universal::BasicString;
