/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/ShareCounted.h>

class IntSharedPointer : public ShareCounted<IntSharedPointer> {
public:
    IntSharedPointer(int value)
        : m_value(value)
    {
    }

    void value(int v) { m_value = v; }

    const int value() const { return m_value; }

private:
    int m_value { 0 };
};
