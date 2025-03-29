/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Universal/Types.h>

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
    WaitBlocker(Process& parent, pid_t& pid, int options)
        : m_parent(parent)
        , m_pid(pid)
        , m_options(options)
    {
    }

    bool is_ready() override;

private:
    Process& m_parent;
    pid_t& m_pid;
    int m_options { 0 };
};
