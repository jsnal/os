/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

#define FORMAT_OK "\x1b[35;1m\x1b[32m"
#define FORMAT_FAIL "\x1b[35;1m\x1b[31m"
#define FORMAT_BOLD "\x1b[1m"
#define FORMAT_RESET "\x1b[0m"

#define CHECK_TRUE(actual)                                 \
    do {                                                   \
        check_equal(name, __LINE__, passed, true, actual); \
    } while (0)

#define CHECK_FALSE(actual)                                 \
    do {                                                    \
        check_equal(name, __LINE__, passed, false, actual); \
    } while (0)

#define CHECK_NULL(actual)                                    \
    do {                                                      \
        check_equal(name, __LINE__, passed, nullptr, actual); \
    } while (0)

#define CHECK_NONNULL(actual)                                     \
    do {                                                          \
        check_not_equal(name, __LINE__, passed, nullptr, actual); \
    } while (0)

#define CHECK_STR_EQUAL(expected, actual)                          \
    do {                                                           \
        check_str_equal(name, __LINE__, passed, expected, actual); \
    } while (0)

#define CHECK_EQUAL(expected, actual)                          \
    do {                                                       \
        check_equal(name, __LINE__, passed, expected, actual); \
    } while (0)

#define CHECK_NOT_EQUAL(expected, actual)                          \
    do {                                                           \
        check_not_equal(name, __LINE__, passed, expected, actual); \
    } while (0)

template<typename A, typename B>
inline void check_equal(const char* name, int line, bool* passed, A expected, B actual)
{
    bool local_passed = expected == actual;
    if (!local_passed) {
        std::cout << FORMAT_FAIL << "FAIL " << FORMAT_RESET << name << ":" << line << " Expected: '" << expected << "' Actual: '" << actual << "'\n";
        *passed = false;
    }
}

template<typename T>
inline void check_equal(const char* name, int line, bool* passed, std::nullptr_t, T actual)
{
    check_equal(name, line, passed, (T) nullptr, actual);
}

template<typename A, typename B>
inline void check_not_equal(const char* name, int line, bool* passed, A expected, B actual)
{
    bool local_passed = expected != actual;
    if (!local_passed) {
        std::cout << FORMAT_FAIL << "FAIL " << FORMAT_RESET << name << ":" << line << " Expected: '" << expected << "' Actual: '" << actual << "'\n";
        *passed = false;
    }
}

template<typename T>
inline void check_not_equal(const char* name, int line, bool* passed, std::nullptr_t, T actual)
{
    check_not_equal(name, line, passed, (T) nullptr, actual);
}

inline void check_str_equal(const char* name, int line, bool* passed, const char* expected, const char* actual)
{
    bool local_passed = true;
    size_t expected_length = strlen(expected);
    size_t actual_length = strlen(actual);

    if (expected_length != actual_length) {
        local_passed = false;
    }

    size_t lower_length = expected_length < actual_length ? expected_length : actual_length;
    for (size_t i = 0; i < lower_length; i++) {
        if (expected[i] != actual[i]) {
            local_passed = false;
        }
    }

    if (!local_passed) {
        std::cout << FORMAT_FAIL << "FAIL " << FORMAT_RESET << name << ":" << line << " Expected: '" << expected << "' Actual: '" << actual << "'\n";
        *passed = false;
    }
}

#define TEST_CASE(test_name) \
    static void test_name(const char* name, bool* passed)

#define ENUMERATE_TEST(name)                                                                                    \
    bool passed_##name = true;                                                                                  \
    name(#name, &passed_##name);                                                                                \
    do {                                                                                                        \
        if (passed_##name) {                                                                                    \
            std::cout << FORMAT_OK << "PASS " << FORMAT_RESET << test_suite_name << "::" << #name << std::endl; \
            tests_passed++;                                                                                     \
        } else {                                                                                                \
            tests_failed++;                                                                                     \
        }                                                                                                       \
    } while (0)

#define TEST_MAIN(test_suite, enumerate_tests)                                                                    \
    int main(void)                                                                                                \
    {                                                                                                             \
        const char* test_suite_name = #test_suite;                                                                \
        int tests_passed = 0;                                                                                     \
        int tests_failed = 0;                                                                                     \
        enumerate_tests();                                                                                        \
        std::cout << test_suite_name << ": Passed: " << tests_passed << " Failed: " << tests_failed << std::endl; \
    }
