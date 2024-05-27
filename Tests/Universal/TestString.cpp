/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/String.h>

TEST_CASE(create)
{
    auto string1 = String("Test");
    CHECK_EQUAL((size_t)4, string1.length());
    CHECK_STR_EQUAL("Test", string1.str());

    auto string2 = String(4);
    CHECK_EQUAL((size_t)4, string2.length());
    CHECK_STR_EQUAL("", string2.str());
}

TEST_CASE(copy_and_move)
{
    auto string1 = String("Test");
    CHECK_EQUAL((size_t)4, string1.length());
    CHECK_STR_EQUAL("Test", string1.str());

    auto string_copy = String(string1);
    CHECK_EQUAL((size_t)4, string_copy.length());
    CHECK_STR_EQUAL(string1.str(), string_copy.str());

    auto string_move1 = String(std::move(string1));
    CHECK_EQUAL((size_t)4, string_move1.length());
    CHECK_EQUAL((size_t)0, string1.length());
    CHECK_TRUE(string1.null());
    CHECK_STR_EQUAL(string_copy.str(), string_move1.str());

    auto string_move2 = std::move(string_move1);
    CHECK_EQUAL((size_t)4, string_move2.length());
    CHECK_EQUAL((size_t)0, string_move1.length());
    CHECK_TRUE(string_move1.null());
    CHECK_STR_EQUAL(string_copy.str(), string_move2.str());
}

TEST_CASE(equals)
{
    auto string1 = String("Same");
    auto string2 = String("Same");
    auto string3 = String("Different");

    const char* cstring1 = "Same";
    const char* cstring2 = "Different";

    CHECK_TRUE(string1 == string2);
    CHECK_TRUE(string1 != string3);
    CHECK_FALSE(string1 == string3);
    CHECK_FALSE(string1 != string2);

    CHECK_TRUE(string1 == cstring1);
    CHECK_TRUE(string1 != cstring2);
    CHECK_FALSE(string1 == cstring2);
    CHECK_FALSE(string1 != cstring1);
}

TEST_CASE(substring)
{
    auto string1 = String("pineapple");
    auto substring1 = string1.substring(0, 3);
    auto substring2 = string1.substring(4, 8);
    auto substring3 = string1.substring(0, 100);

    CHECK_EQUAL((size_t)4, substring1.length());
    CHECK_STR_EQUAL("pine", substring1.str());
    CHECK_EQUAL((size_t)5, substring2.length());
    CHECK_STR_EQUAL("apple", substring2.str());
    CHECK_EQUAL((size_t)0, substring3.length());
    CHECK_TRUE(substring3.null());
}

TEST_CASE(split)
{
    auto string = String("some/path/to/a/file");
    auto split_string = string.split('/');

    CHECK_EQUAL((size_t)5, split_string.size());
    CHECK_STR_EQUAL("some", split_string[0].str());
    CHECK_STR_EQUAL("path", split_string[1].str());
    CHECK_STR_EQUAL("to", split_string[2].str());
    CHECK_STR_EQUAL("a", split_string[3].str());
    CHECK_STR_EQUAL("file", split_string[4].str());
}

TEST_CASE(index_of)
{
    auto string1 = String("some/path/to/a/file");

    CHECK_EQUAL((size_t)4, string1.index_of('/').value());
    CHECK_EQUAL((size_t)14, string1.last_index_of('/').value());
    CHECK_TRUE(string1.last_index_of('-').is_error());
}

TEST_MAIN(TestString, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(copy_and_move);
    ENUMERATE_TEST(equals);
    ENUMERATE_TEST(substring);
    ENUMERATE_TEST(split);
})
