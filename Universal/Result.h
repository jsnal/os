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

#define TRY(expr)                \
    ({                           \
        auto&& result = (expr);  \
        if (result.is_error()) { \
            return result;       \
        }                        \
    })

#define TRY_TAKE(expr)             \
    ({                             \
        auto&& result = (expr);    \
        if (result.is_error()) {   \
            return result.error(); \
        }                          \
        result.release_value();    \
    })

#define MUST(expr)              \
    ({                          \
        auto&& result = (expr); \
        ASSERT(result.is_ok()); \
    })

#define MUST_TAKE(expr)         \
    ({                          \
        auto&& result = (expr); \
        ASSERT(result.is_ok()); \
        result.release_value(); \
    })

enum Status {
    OK = 0,
    Failure = -1,
};

class Result {
public:
    constexpr Result(Status status)
        : m_status(status)
    {
    }

    constexpr Result(i32 status)
        : m_status(status)
    {
    }

    constexpr operator int() const { return m_status; }
    constexpr bool is_ok() const { return m_status == 0; }
    constexpr bool is_error() const { return m_status != 0; }

private:
    i32 m_status { 0 };
};

template<typename T>
class ResultAnd {
public:
    constexpr ResultAnd()
        : m_result(Status::Failure)
    {
    }

    constexpr ResultAnd(Result result)
        : m_result(result)
    {
    }

    constexpr ResultAnd(Status status)
        : m_result(status)
    {
    }

    ResultAnd(T value)
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

    T release_value()
    {
        return m_value.release_value();
    }

private:
    Optional<T> m_value;
    Result m_result;
};
}

using Universal::Result;
using Universal::ResultAnd;
using Universal::Status;
