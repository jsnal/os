/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

class Blocker {
public:
    virtual ~Blocker() { }
    virtual bool is_ready() = 0;
};

class BooleanBlocker final : public Blocker {
public:
    BooleanBlocker() { }
    bool is_ready() override { return m_ready; }
    void ready(bool ready) { m_ready = ready; }

private:
    bool m_ready { false };
};

class WaitBlocker final : public Blocker {
public:
    WaitBlocker() { }

    bool is_ready() override;
};
