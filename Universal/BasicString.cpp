/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/BasicString.h>
#include <Universal/Malloc.h>
#include <Universal/PrintFormat.h>

template<typename CharT>
BasicString<CharT> BasicString<CharT>::format(const char* format, ...)
{
    // TODO: Allow for arbitrary length
    char string[512];
    va_list ap;
    va_start(ap, format);
    print_format_buffer(string, 512, format, ap);
    va_end(ap);
    return string;
}

template<typename CharT>
bool BasicString<CharT>::operator==(const BasicString& other) const
{
    for (size_t i = 0; i < m_length; i++) {
        if (data()[i] != other[i]) {
            return false;
        }
    }
    return true;
}

template<typename CharT>
bool BasicString<CharT>::operator==(const CharT* other) const
{
    if (other == nullptr) {
        return false;
    }

    for (size_t i = 0; i < m_length; i++) {
        if (data()[i] != other[i]) {
            return false;
        }
    }
    return true;
}

template<typename CharT>
BasicString<CharT> BasicString<CharT>::substring(size_t start, size_t end) const
{
    if (start < 0 || end > m_length) {
        return {};
    }

    // TODO: Make this more memory effecient by trimming extra space
    BasicString<CharT> ss(*this);
    ss.m_length = end - start + 1;
    for (size_t i = 0; i < ss.m_length; i++) {
        ss.data()[i] = data()[start + i];
    }
    ss.data()[ss.m_length] = '\0';

    return ss;
}

template<typename CharT>
ArrayList<BasicString<CharT>> BasicString<CharT>::split(char delimiter) const
{
    if (is_empty()) {
        return {};
    }

    ArrayList<BasicString<CharT>> split_array;
    size_t previous_split_start = 0;
    for (size_t i = 0; i < m_length; i++) {
        if (data()[i] == delimiter) {
            size_t split_length = i - previous_split_start;
            if (split_length != 0) {
                split_array.add_last(substring(previous_split_start, i - 1));
            }

            previous_split_start = i + 1;
        }
    }

    size_t leftover_length = m_length - previous_split_start;
    if (leftover_length != 0) {
        split_array.add_last(substring(previous_split_start, m_length - 1));
    }

    return split_array;
}

template<typename CharT>
void BasicString<CharT>::concat(const CharT* other, size_t other_length)
{
    ensure_capacity(m_length + other_length);
    m_length += other_length;
    memcpy(data() + (m_length - other_length), other, other_length);
    data()[m_length] = '\0';
}

template<typename CharT>
void BasicString<CharT>::ensure_capacity(size_t capacity)
{
    if (is_inlined()) {
        if (capacity <= kInlineCapacity) {
            return;
        }

        // Ran out of room in the inline buffer. Switch to the heap
        m_capacity = capacity + (capacity / 2);
        m_data = new CharT[m_capacity + 1];
        memcpy(m_data, m_inline_data, m_length * sizeof(CharT));
        m_data[m_length] = '\0';
    } else if (capacity > m_capacity) {
        m_capacity = m_capacity + (m_capacity / 2);
        if (m_capacity < capacity) {
            m_capacity = capacity;
        }
        CharT* new_data = new CharT[m_capacity + 1];
        memcpy(new_data, m_data, m_length * sizeof(CharT));
        delete[] m_data;
        m_data = new_data;
    }
}
