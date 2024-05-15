/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Stdlib.h>
#include <Universal/String.h>

String::String(const char* string)
    : m_length(strlen(string))
{
    m_data = new char[m_length + 1];
    memcpy(m_data, string, m_length + 1);
}

String::String(size_t length)
    : m_length(length)
{
    m_data = new char[m_length + 1];
}

String::String(const String& other)
    : m_length(other.m_length)
{
    m_data = new char[m_length + 1];
    for (size_t i = 0; i < m_length + 1; i++) {
        m_data[i] = other.m_data[i];
    }
}

String::String(String&& other)
    : m_length(other.m_length)
{
    m_data = other.m_data;
    other.m_data = nullptr;
    other.m_length = 0;
}

String::~String()
{
    delete[] m_data;
}

String& String::operator=(String&& other)
{
    delete[] m_data;
    m_length = other.m_length;
    m_data = other.m_data;
    other.m_data = nullptr;
    other.m_length = 0;

    return *this;
}

bool String::operator==(const String& other) const
{
    for (size_t i = 0; i < m_length; i++) {
        if (m_data[i] != other.m_data[i]) {
            return false;
        }
    }
    return true;
}

bool String::operator==(const char* other) const
{
    for (size_t i = 0; i < m_length; i++) {
        if (m_data[i] != other[i]) {
            return false;
        }
    }
    return true;
}

String String::substring(size_t start, size_t end) const
{
    if (start < 0 || end > m_length) {
        return String();
    }

    String other = String(end - start + 1);
    for (size_t i = 0; i < other.m_length; i++) {
        other.m_data[i] = m_data[start + i];
    }
    other.m_data[other.m_length] = '\0';

    return other;
}
