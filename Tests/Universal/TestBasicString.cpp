/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/BasicString.h>
#include <Universal/Stdlib.h>

TEST_CASE(create)
{
    BasicString<char> empty_string;
    CHECK_EQUAL(0, empty_string.length());

    BasicString<char> s("Small");
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.str());

    BasicString<char> l("Larger string heap allocation");
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.str());
}

TEST_CASE(move_and_copy)
{
    BasicString<char> s("Small");
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.str());

    BasicString<char> s_copy(s);
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.str());
    CHECK_EQUAL(5, s_copy.length());
    CHECK_STR_EQUAL("Small", s_copy.str());

    BasicString<char> s_move(move(s));
    CHECK_EQUAL(0, s.length());
    CHECK_STR_EQUAL("", s.str());
    CHECK_EQUAL(5, s_move.length());
    CHECK_STR_EQUAL("Small", s_move.str());

    BasicString<char> l("Larger string heap allocation");
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.str());

    BasicString<char> l_copy(l);
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.str());
    CHECK_EQUAL(29, l_copy.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_copy.str());

    BasicString<char> l_move(move(l));
    CHECK_EQUAL(0, l.length());
    CHECK_STR_EQUAL("", l.str());
    CHECK_EQUAL(29, l_move.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_move.str());
}

TEST_MAIN(TestString, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(move_and_copy);
})
