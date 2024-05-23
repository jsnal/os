
/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/Optional.h>

TEST_CASE(create)
{
    auto optional1 = Optional<int>();
    CHECK_FALSE(optional1.has_value());

    auto optional2 = Optional<int>(5);
    CHECK_TRUE(optional2.has_value());
    CHECK_EQUAL(5, optional2.value());
}

TEST_CASE(copy_and_move)
{
    auto optional1 = Optional<int>(4);
    auto optional2 = Optional<int>(5);

    auto optional1_copy = Optional<int>();
    optional1_copy = optional1;
    CHECK_TRUE(optional1_copy.has_value());
    CHECK_EQUAL(4, optional1_copy.value());
    CHECK_TRUE(optional1.has_value());
    CHECK_EQUAL(4, optional1.value());

    auto optional2_copy = Optional(optional2);
    CHECK_TRUE(optional2_copy.has_value());
    CHECK_EQUAL(5, optional2_copy.value());

    auto optional1_move = Optional<int>();
    optional1_move = move(optional1);
    CHECK_TRUE(optional1_move.has_value());
    CHECK_EQUAL(4, optional1_move.value());
    CHECK_FALSE(optional1.has_value());

    auto optional2_move = move(optional2);
    CHECK_FALSE(optional2.has_value());
    CHECK_TRUE(optional2_move.has_value());
    CHECK_EQUAL(5, optional2_move.value());
}

TEST_CASE(heap)
{
    auto optional1 = Optional<int*>();
    auto int1 = new int(5);
    optional1 = int1;

    CHECK_TRUE(optional1.has_value());
    CHECK_EQUAL(5, *optional1.value());

    optional1.reset();

    CHECK_FALSE(optional1.has_value());
    delete int1;
}

TEST_MAIN(TestString, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(copy_and_move);
    ENUMERATE_TEST(heap);
})
