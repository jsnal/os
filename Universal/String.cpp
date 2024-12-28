/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/PrintFormat.h>
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

String String::format(const char* format, ...)
{
    // TODO: Allow for arbitrary length
    char message[512];
    va_list ap;

    va_start(ap, format);
    print_format_buffer(message, 512, format, ap);
    va_end(ap);

    return String(message);
}

String& String::operator=(const String& other)
{
    if (this != &other) {
        m_length = other.m_length;
        m_data = new char[m_length + 1];
        for (size_t i = 0; i < m_length + 1; i++) {
            m_data[i] = other.m_data[i];
        }
    }

    return *this;
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

ArrayList<String> String::split(char delimiter) const
{
    if (empty()) {
        return {};
    }

    ArrayList<String> split_array;
    size_t previous_split_start = 0;
    for (size_t i = 0; i < m_length; i++) {
        if (m_data[i] == delimiter) {
            size_t split_length = i - previous_split_start;
            if (split_length != 0) {
                split_array.add_last(substring(previous_split_start, i - 1));
            }

            previous_split_start = i + 1;
        }
    }

    size_t leftover_length = m_length - previous_split_start;
    if (leftover_length != 0) {
        split_array.add_last(substring(previous_split_start, m_length));
    }

    return split_array;
}

ResultReturn<size_t> String::index_of(char ch) const
{
    for (size_t i = 0; i < m_length; i++) {
        if (m_data[i] == ch) {
            return i;
        }
    }

    return Result::Failure;
}

ResultReturn<size_t> String::last_index_of(char ch) const
{
    for (size_t i = m_length - 1; i <= 0; i--) {
        if (m_data[i] == ch) {
            return i;
        }
    }

    return Result::Failure;
}
