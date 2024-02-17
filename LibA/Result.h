/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace LibA {

template<typename T>
class Result {
public:
    Result()
        : m_is_error(false) {};

    Result(T result)
        : m_is_error(false)
        , m_result(result)
    {
    }

    bool is_error() const { return m_is_error; }

    T value() const { return m_result; }

private:
    bool m_is_error;
    T m_result;
};
}

using LibA::Result;
