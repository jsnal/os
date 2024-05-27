/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/ArrayList.h>
#include <Universal/Types.h>

namespace Universal {
class String {
public:
    String() { }

    String(const char*);

    String(size_t length);

    String(const String&);

    String(String&&);

    ~String();

    String& operator=(const String&);
    String& operator=(String&&);

    bool null() const { return m_data == nullptr; }
    bool empty() const { return m_length == 0; }

    char operator[](size_t index) const { return m_data[index]; }

    bool operator==(const String&) const;
    bool operator!=(const String& other) const { return !(*this == other); };

    bool operator==(const char*) const;
    bool operator!=(const char* other) const { return !(*this == other); };

    String substring(size_t start, size_t end) const;

    ArrayList<String> split(char delimiter = ' ') const;

    ResultReturn<size_t> index_of(char ch) const;
    ResultReturn<size_t> last_index_of(char ch) const;

    const char* str() const { return m_data; }

    size_t length() const { return m_length; }

private:
    char* m_data { nullptr };

    size_t m_length { 0 };
};

}

using Universal::String;
