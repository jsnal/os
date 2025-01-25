/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define ENABLE_COLOR 1

#if ENABLE_COLOR
#    define FORMAT_DEBUG_KERNEL "\x1b[35;1m\x1b[35m"
#    define FORMAT_DEBUG_USER "\x1b[36;1m\x1b[36m"
#    define FORMAT_BOLD "\x1b[1m"
#    define FORMAT_RESET "\x1b[0m"
#else
#    define FORMAT_DEBUG ""
#    define FORMAT_INFO ""
#    define FORMAT_WARN ""
#    define FORMAT_ERR ""
#    define FORMAT_BOLD ""
#    define FORMAT_RESET ""
#endif

#ifdef KERNEL
#    include <Kernel/Devices/PIT.h>
#    include <Kernel/kprintf.h>

#    define dbgprintf(tag, format, ...)                                                                                                             \
        do {                                                                                                                                        \
            kprintf(FORMAT_BOLD "[Kernel:%u]:" FORMAT_DEBUG_KERNEL "%s: " FORMAT_RESET format, PIT::milliseconds_since_boot(), tag, ##__VA_ARGS__); \
        } while (0)

#    define dbgprintln(tag, format, ...)                                                                                                                 \
        do {                                                                                                                                             \
            kprintf(FORMAT_BOLD "[Kernel:%u]:" FORMAT_DEBUG_KERNEL "%s: " FORMAT_RESET format "\n", PIT::milliseconds_since_boot(), tag, ##__VA_ARGS__); \
        } while (0)

#    define dbgprintf_if(condition, tag, format, ...) \
        if constexpr (condition) {                    \
            dbgprintf(tag, format, ##__VA_ARGS__);    \
        }

#    define dbgprintln_if(condition, tag, format, ...) \
        if constexpr (condition) {                     \
            dbgprintln(tag, format, ##__VA_ARGS__);    \
        }
#else
#    include <stdio.h>

#    define dbgprintf(tag, format, ...)                                                                             \
        do {                                                                                                        \
            udbgprintf(FORMAT_BOLD "[Userland]:" FORMAT_DEBUG_USER "%s: " FORMAT_RESET format, tag, ##__VA_ARGS__); \
        } while (0)

#    define dbgprintln(tag, format, ...)                                                                                 \
        do {                                                                                                             \
            udbgprintf(FORMAT_BOLD "[Userland]:" FORMAT_DEBUG_USER "%s: " FORMAT_RESET format "\n", tag, ##__VA_ARGS__); \
        } while (0)

#    define dbgprintf_if(condition, tag, format, ...) \
        if constexpr (condition) {                    \
            dbgprintf(tag, format, ##__VA_ARGS__);    \
        }

#    define dbgprintln_if(condition, tag, format, ...) \
        if constexpr (condition) {                     \
            dbgprintln(tag, format, ##__VA_ARGS__);    \
        }
#endif
