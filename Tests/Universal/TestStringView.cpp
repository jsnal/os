/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/StringView.h>

TEST_CASE(create)
{
    StringView sv("Test");
    CHECK_EQUAL(4, sv.length());
    CHECK_FALSE(sv.is_empty());
    CHECK_STR_EQUAL("Test", sv.str());

    const char* raw_str = "Raw String";
    StringView sv2(raw_str);
    CHECK_EQUAL(10, sv2.length());
    CHECK_FALSE(sv2.is_empty());
    CHECK_STR_EQUAL("Raw String", sv2.str());

    StringView sv3(nullptr);
    CHECK_EQUAL(0, sv3.length());
    CHECK_TRUE(sv3.is_empty());
    CHECK_TRUE(sv3.is_null());

    auto sv_suffix = "Test"sv;
    CHECK_EQUAL(4, sv_suffix.length());
    CHECK_FALSE(sv_suffix.is_empty());
    CHECK_STR_EQUAL("Test", sv_suffix.str());
}

TEST_CASE(equals)
{
    StringView sv("Test");
    StringView sv_same("Test");
    StringView sv_diff("Not Test");
    StringView sv_empty;

    CHECK_TRUE(sv == sv_same);
    CHECK_TRUE(sv_empty == sv_empty);
    CHECK_FALSE(sv == sv_diff);
    CHECK_FALSE(sv == sv_empty);

    CHECK_FALSE(sv != sv_same);
    CHECK_FALSE(sv_empty != sv_empty);
    CHECK_TRUE(sv != sv_diff);
    CHECK_TRUE(sv != sv_empty);
}

TEST_CASE(index)
{
    StringView sv("Longer string to index");

    CHECK_EQUAL('s', sv.get(7));
    CHECK_EQUAL('s', sv[7]);
    CHECK_EQUAL('L', sv.get(0));
    CHECK_EQUAL('L', sv[0]);
    CHECK_EQUAL('x', sv.get(21));
    CHECK_EQUAL('x', sv[21]);
    CHECK_NOT_EQUAL('x', sv.get(10));
    CHECK_NOT_EQUAL('x', sv[10]);
}

TEST_CASE(copy_and_move)
{
    StringView sv("Test");

    StringView sv_copy_constructor(sv);
    CHECK_EQUAL(4, sv.length());
    CHECK_STR_EQUAL("Test", sv.str());
    CHECK_EQUAL(4, sv_copy_constructor.length());
    CHECK_STR_EQUAL("Test", sv_copy_constructor.str());

    StringView sv_copy_assignment = sv;
    CHECK_EQUAL(4, sv.length());
    CHECK_STR_EQUAL("Test", sv.str());
    CHECK_EQUAL(4, sv_copy_assignment.length());
    CHECK_STR_EQUAL("Test", sv_copy_assignment.str());

    StringView sv_move_constructor(move(sv));
    CHECK_EQUAL(0, sv.length());
    CHECK_TRUE(sv.is_null());
    CHECK_EQUAL(4, sv_move_constructor.length());
    CHECK_STR_EQUAL("Test", sv_move_constructor.str());

    StringView sv_move_assignment = move(sv_move_constructor);
    CHECK_EQUAL(0, sv_move_constructor.length());
    CHECK_TRUE(sv_move_constructor.is_null());
    CHECK_EQUAL(4, sv_move_assignment.length());
    CHECK_STR_EQUAL("Test", sv_move_assignment.str());
}

TEST_CASE(front_and_back)
{
    auto sv = "Test"sv;
    CHECK_EQUAL(4, sv.length());
    CHECK_EQUAL('T', sv.front());
    CHECK_EQUAL('t', sv.back());
}

TEST_CASE(contains)
{
    auto sv = "apple banana orange"sv;
    CHECK_FALSE(sv.contains("grape"sv));
    CHECK_TRUE(sv.contains("apple"sv));
    CHECK_TRUE(sv.contains("apple "sv));
    CHECK_FALSE(sv.contains("orange "sv));
    CHECK_TRUE(sv.contains("banana orange"sv));

    StringView sv_null(nullptr);
    CHECK_FALSE(sv_null.contains("grape"sv));
    CHECK_FALSE(sv.contains(sv_null));

    StringView sv_empty("");
    CHECK_TRUE(sv.contains(sv_empty));
    CHECK_FALSE(sv_empty.contains("anything"));
}

TEST_CASE(find)
{
    auto sv = "apple banana orange"sv;
    CHECK_EQUAL(6, sv.find('b').value());
    CHECK_EQUAL(4, sv.find('e').value());
    CHECK_EQUAL(18, sv.find_last('e').value());
    CHECK_EQUAL(6, sv.find_last('b').value());
    CHECK_FALSE(sv.find('z').has_value());
    CHECK_FALSE(sv.find_last('z').has_value());

    StringView sv_null(nullptr);
    CHECK_FALSE(sv_null.find('a').has_value());
    CHECK_FALSE(sv_null.find_last('a').has_value());

    StringView sv_empty("");
    CHECK_FALSE(sv_empty.find('a').has_value());
    CHECK_FALSE(sv_empty.find_last('a').has_value());
}

TEST_MAIN(TestStringView, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(equals);
    ENUMERATE_TEST(index);
    ENUMERATE_TEST(copy_and_move);
    ENUMERATE_TEST(front_and_back);
    ENUMERATE_TEST(contains);
    ENUMERATE_TEST(find);
})
