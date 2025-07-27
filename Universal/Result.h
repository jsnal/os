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

#define MUST(expr)              \
    ({                          \
        auto&& result = (expr); \
        ASSERT(result.is_ok()); \
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

}

using Universal::Result;
using Universal::Status;
