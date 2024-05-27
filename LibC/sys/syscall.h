/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

int syscall(int call);
int syscall(int call, int arg1);
int syscall(int call, int arg1, int arg2);
int syscall(int call, int arg1, int arg2, int arg3);
int syscall_no_errno(int call);
