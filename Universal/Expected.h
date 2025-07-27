/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/Result.h>

namespace Universal {

#define TRY_TAKE(expr)             \
    ({                             \
        auto&& result = (expr);    \
        if (result.is_error()) {   \
            return result.error(); \
        }                          \
        result.release_value();    \
    })

#define MUST_TAKE(expr)         \
    ({                          \
        auto&& result = (expr); \
        ASSERT(result.is_ok()); \
        result.release_value(); \
    })

template<typename T>
class Expected {
public:
    constexpr Expected()
        : m_result(Status::Failure)
    {
    }

    constexpr Expected(Result result)
        : m_result(result)
    {
    }

    constexpr Expected(Status status)
        : m_result(status)
    {
    }

    Expected(T value)
        : m_value(move(value))
        , m_result(Status::OK)
    {
    }

    constexpr bool is_ok() const { return m_result.is_ok(); }
    constexpr bool is_error() const { return m_result.is_error(); }
    constexpr Result error() const { return m_result; }

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

    T release_value() { return m_value.release_value(); }

private:
    Optional<T> m_value;
    Result m_result;
};

}

using Universal::Expected;
