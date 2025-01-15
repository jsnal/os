/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/StringView.h>

bool StringView::contains(const StringView& needle) const
{
    if (is_empty() || is_null() || needle.is_null() || needle.length() > m_length) {
        return false;
    }

    if (needle.is_empty()) {
        return true;
    }

    for (size_t si = 0; si < m_length; si++) {
        if (get(si) != needle[0]) {
            continue;
        }

        for (size_t ni = 0; si + ni < length(); ni++) {
            if (get(si + ni) != needle[ni]) {
                break;
            }
            if (ni + 1 == needle.length()) {
                return true;
            }
        }
    }

    return false;
}

Optional<size_t> StringView::find(char needle) const
{
    for (size_t i = 0; i < m_length; i++) {
        if (get(i) == needle) {
            return i;
        }
    }

    return {};
}

Optional<size_t> StringView::find_last(char needle) const
{
    for (size_t i = m_length; i > 0; i--) {
        if (get(i - 1) == needle) {
            return i - 1;
        }
    }

    return {};
}
