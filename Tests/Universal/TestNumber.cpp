/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/Number.h>

TEST_CASE(number_between)
{
    CHECK_TRUE(number_between(2, 1, 3));
    CHECK_TRUE(number_between(1.5, 1, 2));
    CHECK_FALSE(number_between(0, 1, 3));
    CHECK_FALSE(number_between(1, 1, 3));
}

TEST_CASE(number_between_inclusive)
{
    CHECK_TRUE(number_between_inclusive(2, 1, 3));
    CHECK_TRUE(number_between_inclusive(1.5, 1, 2));
    CHECK_FALSE(number_between_inclusive(0, 1, 3));
    CHECK_TRUE(number_between_inclusive(1, 1, 3));
}

TEST_CASE(ceiling_divide)
{
    CHECK_EQUAL(1, ceiling_divide(1, 2));
    CHECK_EQUAL(2, ceiling_divide(3, 2));
}

TEST_MAIN(TestNumber, [&]() {
    ENUMERATE_TEST(ceiling_divide);
    ENUMERATE_TEST(number_between_inclusive);
    ENUMERATE_TEST(number_between);
})
