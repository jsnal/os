/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

class User {
public:
    static User root()
    {
        return User(0);
    }

    User(uid_t uid)
        : m_uid(uid)
        , m_euid(uid)
        , m_gid(uid)
        , m_egid(uid)
    {
    }

    uid_t uid() const { return m_uid; }
    uid_t euid() const { return m_euid; }
    uid_t gid() const { return m_gid; }
    uid_t egid() const { return m_egid; }

private:
    uid_t m_uid { 0 };
    uid_t m_euid { 0 };
    gid_t m_gid { 0 };
    gid_t m_egid { 0 };
};
