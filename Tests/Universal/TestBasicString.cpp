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
    CHECK_STR_EQUAL("Small", s.data());

    BasicString<char> s_trimmed("Trim-----", 4);
    CHECK_EQUAL(4, s_trimmed.length());
    CHECK_STR_EQUAL("Trim", s_trimmed.data());

    BasicString<char> l("Larger string heap allocation");
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.data());

    StringView sv = "StringView"sv;
    BasicString<char> from_sv(sv);
    CHECK_EQUAL(10, from_sv.length());
    CHECK_STR_EQUAL("StringView", from_sv.data());
}

TEST_CASE(move_and_copy)
{
    BasicString<char> s("Small");
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.data());

    BasicString<char> s_constructor_copy(s);
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.data());
    CHECK_EQUAL(5, s_constructor_copy.length());
    CHECK_STR_EQUAL("Small", s_constructor_copy.data());

    BasicString<char> s_equals_copy = s;
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.data());
    CHECK_EQUAL(5, s_equals_copy.length());
    CHECK_STR_EQUAL("Small", s_equals_copy.data());

    BasicString<char> s_constructor_move(move(s_constructor_copy));
    CHECK_EQUAL(0, s_constructor_copy.length());
    CHECK_STR_EQUAL("", s_constructor_copy.data());
    CHECK_EQUAL(5, s_constructor_move.length());
    CHECK_STR_EQUAL("Small", s_constructor_move.data());

    BasicString<char> s_equals_move = move(s_equals_copy);
    CHECK_EQUAL(0, s_equals_copy.length());
    CHECK_STR_EQUAL("", s_equals_copy.data());
    CHECK_EQUAL(5, s_equals_move.length());
    CHECK_STR_EQUAL("Small", s_equals_move.data());

    BasicString<char> l("Larger string heap allocation");
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.data());

    BasicString<char> l_constructor_copy(l);
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.data());
    CHECK_EQUAL(29, l_constructor_copy.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_constructor_copy.data());

    BasicString<char> l_equals_copy = l;
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.data());
    CHECK_EQUAL(29, l_equals_copy.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_equals_copy.data());

    BasicString<char> l_constructor_move(move(l_constructor_copy));
    CHECK_EQUAL(0, l_constructor_copy.length());
    CHECK_STR_EQUAL("", l_constructor_copy.data());
    CHECK_EQUAL(29, l_constructor_move.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_constructor_move.data());

    BasicString<char> l_equals_move = move(l_equals_copy);
    CHECK_EQUAL(0, l_equals_copy.length());
    CHECK_STR_EQUAL("", l_equals_copy.data());
    CHECK_EQUAL(29, l_equals_move.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_equals_move.data());
}

TEST_CASE(equals)
{
    BasicString<char> string1("Same");
    BasicString<char> string2("Same");
    BasicString<char> string3("Different");
    BasicString<char> string4("Larger string heap allocation");
    BasicString<char> string5("Larger string heap allocation");

    const char* cstring1 = "Same";
    const char* cstring2 = "Different";

    CHECK_TRUE(string1 == string2);
    CHECK_TRUE(string1 != string3);
    CHECK_FALSE(string1 == string3);
    CHECK_FALSE(string1 != string2);
    CHECK_FALSE(string4 == string1);
    CHECK_TRUE(string4 == string5);

    CHECK_TRUE(string1 == cstring1);
    CHECK_TRUE(string1 != cstring2);
    CHECK_FALSE(string1 == cstring2);
    CHECK_FALSE(string1 != cstring1);
}

TEST_CASE(concat)
{
    BasicString<char> concat_method("string1");
    BasicString<char> string2(" string2");
    const char* cstring1 = " string3";
    StringView sv1 = " string4"sv;

    concat_method.concat(string2);
    CHECK_STR_EQUAL("string1 string2", concat_method.data());

    concat_method.concat(cstring1);
    CHECK_STR_EQUAL("string1 string2 string3", concat_method.data());

    concat_method.concat(sv1);
    CHECK_STR_EQUAL("string1 string2 string3 string4", concat_method.data());

    BasicString<char> concat_plus_equals("string1");

    concat_plus_equals += string2;
    CHECK_STR_EQUAL("string1 string2", concat_plus_equals.data());

    concat_plus_equals += cstring1;
    CHECK_STR_EQUAL("string1 string2 string3", concat_plus_equals.data());

    concat_plus_equals += sv1;
    CHECK_STR_EQUAL("string1 string2 string3 string4", concat_plus_equals.data());

    BasicString<char> concat_plus("string1");

    concat_plus = concat_plus + string2;
    CHECK_STR_EQUAL("string1 string2", concat_plus.data());

    concat_plus = concat_plus + cstring1;
    CHECK_STR_EQUAL("string1 string2 string3", concat_plus.data());

    concat_plus = concat_plus + sv1;
    CHECK_STR_EQUAL("string1 string2 string3 string4", concat_plus.data());

    BasicString<char> concat_plus_chain("string1");
    concat_plus_chain = concat_plus_chain + string2 + cstring1 + sv1;
    CHECK_STR_EQUAL("string1 string2 string3 string4", concat_plus_chain.data());

    BasicString<char> concat_plus_chain_huge("string1");
    concat_plus_chain_huge = concat_plus_chain_huge + string2 + cstring1 + sv1 + " really long string that should reallocate";
    CHECK_STR_EQUAL("string1 string2 string3 string4 really long string that should reallocate", concat_plus_chain_huge.data());
}

TEST_CASE(string_view)
{
    BasicString<char> empty_string;
    StringView empty_sv = empty_string.string_view();
    CHECK_TRUE(empty_sv.is_empty());

    BasicString<char> s("Small");
    StringView s_sv = s.string_view();
    CHECK_EQUAL(5, s_sv.length());
    CHECK_STR_EQUAL("Small", s_sv.str());

    BasicString<char> l("Larger string heap allocation");
    StringView l_sv = l.string_view();
    CHECK_EQUAL(29, l_sv.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l_sv.str());
}

TEST_CASE(substring)
{
    BasicString<char> string1("pineapple");
    auto substring1 = string1.substring(0, 3);
    auto substring2 = string1.substring(4, 8);
    auto substring3 = string1.substring(0, 100);

    CHECK_EQUAL((size_t)4, substring1.length());
    CHECK_STR_EQUAL("pine", substring1.data());
    CHECK_EQUAL((size_t)5, substring2.length());
    CHECK_STR_EQUAL("apple", substring2.data());
    CHECK_EQUAL((size_t)0, substring3.length());
    CHECK_TRUE(substring3.is_empty());
}

TEST_CASE(contains)
{
    BasicString<char> string1("apple banana orange");
    CHECK_FALSE(string1.contains("grape"sv));
    CHECK_TRUE(string1.contains("apple"sv));
    CHECK_TRUE(string1.contains("apple "));
    CHECK_FALSE(string1.contains("orange "sv));
    CHECK_TRUE(string1.contains(BasicString<char>("banana orange")));

    BasicString<char> string_null(nullptr);
    CHECK_FALSE(string_null.contains("grape"sv));
    CHECK_TRUE(string1.contains(string_null));

    BasicString<char> string_empty("");
    CHECK_TRUE(string1.contains(string_empty));
    CHECK_FALSE(string_empty.contains("anything"));
}

TEST_CASE(find)
{
    BasicString<char> string1("apple banana orange");
    CHECK_EQUAL(6, string1.find('b').value());
    CHECK_EQUAL(4, string1.find('e').value());
    CHECK_EQUAL(18, string1.find_last('e').value());
    CHECK_EQUAL(6, string1.find_last('b').value());
    CHECK_FALSE(string1.find('z').has_value());
    CHECK_FALSE(string1.find_last('z').has_value());

    StringView sv_null(nullptr);
    CHECK_FALSE(sv_null.find('a').has_value());
    CHECK_FALSE(sv_null.find_last('a').has_value());

    StringView sv_empty("");
    CHECK_FALSE(sv_empty.find('a').has_value());
    CHECK_FALSE(sv_empty.find_last('a').has_value());
}

TEST_CASE(split)
{
    BasicString<char> string1("some/path/to/a/file");
    auto split_string = string1.split('/');

    CHECK_EQUAL((size_t)5, split_string.size());
    CHECK_STR_EQUAL("some", split_string[0].data());
    CHECK_STR_EQUAL("path", split_string[1].data());
    CHECK_STR_EQUAL("to", split_string[2].data());
    CHECK_STR_EQUAL("a", split_string[3].data());
    CHECK_STR_EQUAL("file", split_string[4].data());
}

TEST_CASE(format)
{
    BasicString<char> s = BasicString<char>::format("Small");
    CHECK_EQUAL(5, s.length());
    CHECK_STR_EQUAL("Small", s.data());

    BasicString<char> l = BasicString<char>::format("Larger string heap allocation");
    CHECK_EQUAL(29, l.length());
    CHECK_STR_EQUAL("Larger string heap allocation", l.data());

    BasicString<char> f = BasicString<char>::format("Number: %d, Hex: %x, String: %s", 10, 0x1337, "My String");
    CHECK_STR_EQUAL("Number: 10, Hex: 1337, String: My String", f.data());
}

TEST_MAIN(TestBasicString, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(move_and_copy);
    ENUMERATE_TEST(equals);
    ENUMERATE_TEST(concat);
    ENUMERATE_TEST(string_view);
    ENUMERATE_TEST(substring);
    ENUMERATE_TEST(contains);
    ENUMERATE_TEST(find);
    ENUMERATE_TEST(split);
    ENUMERATE_TEST(format);
})
