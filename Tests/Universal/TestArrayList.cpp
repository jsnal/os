/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/ArrayList.h>

TEST_CASE(create)
{
    auto array_list = ArrayList<u32>();
    CHECK_EQUAL((size_t)0, array_list.size());
    CHECK_EQUAL((size_t)1, array_list.capacity());
}

TEST_CASE(add)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.empty());

    array_list.add(0, 5);
    CHECK_EQUAL((size_t)1, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)5, array_list[0]);

    array_list.add(1, 10);
    CHECK_EQUAL((size_t)2, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)10, array_list[1]);

    array_list.add(0, 0);
    CHECK_EQUAL((size_t)3, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)0, array_list[0]);
    CHECK_EQUAL((u32)5, array_list[1]);
    CHECK_EQUAL((u32)10, array_list[2]);
}

TEST_CASE(remove)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);
    CHECK_EQUAL((size_t)4, array_list.size());
    CHECK_FALSE(array_list.empty());

    CHECK_TRUE(array_list.remove(100).is_error());

    CHECK_TRUE(array_list.remove(1).is_ok());
    CHECK_EQUAL((size_t)3, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)15, array_list[1]);
    CHECK_EQUAL((u32)20, array_list[2]);

    CHECK_TRUE(array_list.remove(1).is_ok());
    CHECK_EQUAL((size_t)2, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)20, array_list[1]);

    CHECK_TRUE(array_list.remove(1).is_ok());
    CHECK_EQUAL((size_t)1, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)5, array_list[0]);

    CHECK_TRUE(array_list.remove(0).is_ok());
    CHECK_EQUAL((size_t)0, array_list.size());
    CHECK_TRUE(array_list.empty());

    CHECK_TRUE(array_list.remove(0).is_error());
}

TEST_CASE(set)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);

    CHECK_TRUE(array_list.set(1, 25).is_ok());
    CHECK_EQUAL((size_t)4, array_list.size());
    CHECK_FALSE(array_list.empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)25, array_list[1]);
    CHECK_EQUAL((u32)15, array_list[2]);
    CHECK_EQUAL((u32)20, array_list[3]);

    CHECK_TRUE(array_list.set(100, 25).is_error());
}

TEST_CASE(get)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);
    CHECK_EQUAL((u32)5, array_list.get(0));
    CHECK_EQUAL((u32)10, array_list.get(1));
    CHECK_EQUAL((u32)15, array_list.get(2));
    CHECK_EQUAL((u32)20, array_list.get(3));
}

TEST_CASE(first_and_last)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);

    CHECK_EQUAL((u32)5, array_list.first());
    CHECK_EQUAL((u32)20, array_list.last());
}

TEST_CASE(pointers)
{
    auto array_list = ArrayList<char*>();
    CHECK_TRUE(array_list.empty());

    array_list.add(0, (char*)"string1");
    array_list.add(1, (char*)"string2");
    array_list.add(2, (char*)"string3");
    array_list.add(3, (char*)"string4");

    CHECK_EQUAL((char*)"string1", array_list[0]);
    CHECK_EQUAL((char*)"string2", array_list[1]);
    CHECK_EQUAL((char*)"string3", array_list[2]);
    CHECK_EQUAL((char*)"string4", array_list[3]);
}

TEST_MAIN(TestArrayList, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(add);
    ENUMERATE_TEST(remove);
    ENUMERATE_TEST(set);
    ENUMERATE_TEST(get);
    ENUMERATE_TEST(first_and_last);
    ENUMERATE_TEST(pointers);
})
