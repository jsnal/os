/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/Optional.h>
#include <Universal/Types.h>

namespace Universal {

class Result {
public:
    constexpr static int OK = 0;
    constexpr static int Failure = 1;

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
class ResultReturn {
public:
    ResultReturn()
        : m_result(1)
    {
    }

    ResultReturn(Result result)
        : m_result(result)
    {
    }

    ResultReturn(T value)
        : m_value(move(value))
        , m_result(Result::OK)
    {
    }

    bool is_ok() const { return m_result.is_ok(); }
    bool is_error() const { return m_result.is_error(); }

    Result error() const { return m_result; }

    T& value()
    {
        ASSERT(!is_error());
        return m_value.value();
    }

    const T& value() const
    {
        ASSERT(!is_error());
        return m_value.value();
    }

private:
    Optional<T> m_value;
    Result m_result;
};
}

using Universal::Result;
using Universal::ResultReturn;
