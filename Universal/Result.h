/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

namespace Universal {

class Result {
public:
    constexpr static int OK = 0;

    Result(int error_value)
        : m_error_value(error_value)
    {
    }

    operator int() const { return m_error_value; }

    bool is_ok() const { return m_error_value == 0; }
    bool is_error() const { return m_error_value != 0; }

private:
    int m_error_value { 0 };
};

template<typename T>
class ResultOr {
public:
    ResultOr()
        : m_result(1)
    {
    }

    ResultOr(Result result)
        : m_result(result)
    {
    }

    ResultOr(T value)
        : m_value(value)
        , m_result(Result::OK)
    {
    }

    bool is_ok() const { return m_result.is_ok(); }
    bool is_error() const { return m_result.is_error(); }

    Result error() const { return m_result; }

    T& value() { return m_value; }
    const T& value() const { return m_value; }

private:
    T m_value;
    Result m_result;
};
}

using Universal::Result;
using Universal::ResultOr;
