/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Tests/Universal/Int.h>
#include <Universal/ArrayList.h>
#include <Universal/SharedPtr.h>

TEST_CASE(create)
{
    auto array_list = ArrayList<u32>();
    CHECK_EQUAL(0, array_list.size());
    CHECK_EQUAL(0, array_list.capacity());
}

TEST_CASE(add)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add(0, 5);
    CHECK_EQUAL((size_t)1, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)5, array_list[0]);

    array_list.add(1, 10);
    CHECK_EQUAL((size_t)2, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)10, array_list[1]);

    array_list.add(0, 0);
    CHECK_EQUAL((size_t)3, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)0, array_list[0]);
    CHECK_EQUAL((u32)5, array_list[1]);
    CHECK_EQUAL((u32)10, array_list[2]);
}

TEST_CASE(remove)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);
    CHECK_EQUAL((size_t)4, array_list.size());
    CHECK_FALSE(array_list.is_empty());

    CHECK_TRUE(array_list.remove(100).is_error());

    CHECK_TRUE(array_list.remove(1).is_ok());
    CHECK_EQUAL((size_t)3, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)15, array_list[1]);
    CHECK_EQUAL((u32)20, array_list[2]);

    CHECK_TRUE(array_list.remove(1).is_ok());
    CHECK_EQUAL((size_t)2, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)20, array_list[1]);

    CHECK_TRUE(array_list.remove(1).is_ok());
    CHECK_EQUAL((size_t)1, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)5, array_list[0]);

    CHECK_TRUE(array_list.remove(0).is_ok());
    CHECK_EQUAL((size_t)0, array_list.size());
    CHECK_TRUE(array_list.is_empty());

    CHECK_TRUE(array_list.remove(0).is_error());
}

TEST_CASE(set)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);

    CHECK_TRUE(array_list.set(1, 25).is_ok());
    CHECK_EQUAL((size_t)4, array_list.size());
    CHECK_FALSE(array_list.is_empty());
    CHECK_EQUAL((u32)5, array_list[0]);
    CHECK_EQUAL((u32)25, array_list[1]);
    CHECK_EQUAL((u32)15, array_list[2]);
    CHECK_EQUAL((u32)20, array_list[3]);

    CHECK_TRUE(array_list.set(100, 25).is_error());
}

TEST_CASE(get)
{
    auto array_list = ArrayList<u32>();
    CHECK_TRUE(array_list.is_empty());

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
    CHECK_TRUE(array_list.is_empty());

    array_list.add(0, 5);
    array_list.add(1, 10);
    array_list.add(2, 15);
    array_list.add(3, 20);

    CHECK_EQUAL((u32)5, array_list.first());
    CHECK_EQUAL((u32)20, array_list.last());
}

TEST_CASE(pointers)
{
    auto array_list = ArrayList<const char*>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add(0, "string1");
    array_list.add(1, "string2");
    array_list.add(2, "string3");
    array_list.add(3, "string4");

    CHECK_EQUAL("string1", array_list[0]);
    CHECK_EQUAL("string2", array_list[1]);
    CHECK_EQUAL("string3", array_list[2]);
    CHECK_EQUAL("string4", array_list[3]);
}

TEST_CASE(shared_pointers)
{
    auto shared_point1 = make_shared_ptr<Int>(1);
    auto shared_point2 = make_shared_ptr<Int>(2);
    auto shared_point3 = make_shared_ptr<Int>(3);
    auto shared_point4 = make_shared_ptr<Int>(4);

    auto array_list = ArrayList<SharedPtr<Int>>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add(0, shared_point1);
    array_list.add(1, shared_point2);
    array_list.add(2, shared_point3);
    array_list.add(3, shared_point4);

    CHECK_EQUAL((int)1, array_list[0]->value());
    CHECK_EQUAL((int)2, array_list[1]->value());
    CHECK_EQUAL((int)3, array_list[2]->value());
    CHECK_EQUAL((int)4, array_list[3]->value());
}

TEST_CASE(sort)
{
    auto array_list = ArrayList<int>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add_last(0);
    array_list.add_last(4);
    array_list.add_last(3);
    array_list.add_last(9);
    array_list.add_last(1);

    array_list.sort([&](int a, int b) {
        return b < a;
    });

    CHECK_EQUAL((int)0, array_list[0]);
    CHECK_EQUAL((int)1, array_list[1]);
    CHECK_EQUAL((int)3, array_list[2]);
    CHECK_EQUAL((int)4, array_list[3]);
    CHECK_EQUAL((int)9, array_list[4]);
}

TEST_CASE(sort_objects)
{
    auto shared_point1 = make_shared_ptr<Int>(1);
    auto shared_point2 = make_shared_ptr<Int>(2);
    auto shared_point3 = make_shared_ptr<Int>(3);
    auto shared_point4 = make_shared_ptr<Int>(4);

    auto array_list = ArrayList<SharedPtr<Int>>();
    CHECK_TRUE(array_list.is_empty());

    array_list.add_last(shared_point2);
    array_list.add_last(shared_point4);
    array_list.add_last(shared_point1);
    array_list.add_last(shared_point3);

    array_list.sort([&](SharedPtr<Int> a, SharedPtr<Int> b) {
        return b->value() < a->value();
    });

    CHECK_EQUAL((int)1, array_list[0]->value());
    CHECK_EQUAL((int)2, array_list[1]->value());
    CHECK_EQUAL((int)3, array_list[2]->value());
    CHECK_EQUAL((int)4, array_list[3]->value());
}

TEST_CASE(move)
{
    ArrayList<int> array_list;
    CHECK_TRUE(array_list.is_empty());

    array_list.add_last(0);
    array_list.add_last(4);
    array_list.add_last(3);

    CHECK_EQUAL((int)0, array_list[0]);
    CHECK_EQUAL((int)4, array_list[1]);
    CHECK_EQUAL((int)3, array_list[2]);

    auto array_list_construct = ArrayList<int>(move(array_list));
    CHECK_FALSE(array_list_construct.is_empty());
    CHECK_EQUAL((size_t)7, array_list_construct.capacity());

    CHECK_EQUAL((int)0, array_list_construct[0]);
    CHECK_EQUAL((int)4, array_list_construct[1]);
    CHECK_EQUAL((int)3, array_list_construct[2]);

    ArrayList<int> array_list_equals;
    CHECK_TRUE(array_list_equals.is_empty());
    CHECK_FALSE(array_list_construct.is_empty());

    array_list_equals = move(array_list_construct);

    CHECK_TRUE(array_list_construct.is_empty());
    CHECK_EQUAL((int)0, array_list_equals[0]);
    CHECK_EQUAL((int)4, array_list_equals[1]);
    CHECK_EQUAL((int)3, array_list_equals[2]);
}

TEST_MAIN(TestArrayList, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(add);
    ENUMERATE_TEST(remove);
    ENUMERATE_TEST(set);
    ENUMERATE_TEST(get);
    ENUMERATE_TEST(first_and_last);
    ENUMERATE_TEST(pointers);
    ENUMERATE_TEST(shared_pointers);
    ENUMERATE_TEST(sort);
    ENUMERATE_TEST(sort_objects);
    ENUMERATE_TEST(move);
})
