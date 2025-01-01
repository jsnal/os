/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Tests/Universal/IntSharedPtr.h>
#include <Universal/Array.h>
#include <Universal/SharedPtr.h>

TEST_CASE(create)
{
    auto array = Array<u32>(10);
    CHECK_EQUAL((size_t)10, array.size());
}

TEST_CASE(add)
{
    auto array = Array<u32>(10);
    CHECK_EQUAL((size_t)10, array.size());

    array[0] = 10;
    array[1] = 9;
    CHECK_EQUAL((u32)10, array[0]);
    CHECK_EQUAL((u32)9, array[1]);
}

TEST_CASE(shared_pointers)
{
    auto shared_point1 = adopt(*new IntSharedPointer(1));
    auto shared_point2 = adopt(*new IntSharedPointer(2));
    auto array = Array<SharedPtr<IntSharedPointer>>(10);
    CHECK_EQUAL((size_t)10, array.size());

    array[0] = shared_point1;
    array[1] = shared_point2;
    CHECK_EQUAL(shared_point1.ptr(), array[0].ptr());
    CHECK_EQUAL(shared_point2.ptr(), array[1].ptr());
    CHECK_EQUAL((int)1, array[0]->value());
    CHECK_EQUAL((int)2, array[1]->value());
}

TEST_CASE(big_array)
{
    auto array = Array<u32>(500);
    CHECK_EQUAL((size_t)500, array.size());

    array[0] = 10;
    array[1] = 9;
    array[300] = 5;
    array[499] = 8;
    CHECK_EQUAL((u32)10, array[0]);
    CHECK_EQUAL((u32)9, array[1]);
    CHECK_EQUAL((u32)5, array[300]);
    CHECK_EQUAL((u32)8, array[499]);
}

TEST_CASE(inlined_data)
{
    auto array = Array<u32, 10>();
    CHECK_EQUAL((size_t)10, array.size());

    array[0] = 10;
    array[1] = 9;
    array[9] = 5;
    CHECK_EQUAL((u32)10, array[0]);
    CHECK_EQUAL((u32)9, array[1]);
    CHECK_EQUAL((u32)5, array[9]);
}

TEST_CASE(copy_and_move)
{
    auto array = Array<u32>(10);
    CHECK_EQUAL((size_t)10, array.size());

    array[0] = 10;
    array[1] = 9;
    array[9] = 5;

    auto array_copy(array);
    CHECK_EQUAL((size_t)10, array.size());
    CHECK_EQUAL((size_t)10, array_copy.size());
    CHECK_EQUAL((u32)10, array[0]);
    CHECK_EQUAL((u32)9, array[1]);
    CHECK_EQUAL((u32)5, array[9]);
    CHECK_EQUAL((u32)10, array_copy[0]);
    CHECK_EQUAL((u32)9, array_copy[1]);
    CHECK_EQUAL((u32)5, array_copy[9]);

    auto array_moved(move(array));
    CHECK_EQUAL((size_t)10, array_moved.size());
    CHECK_EQUAL((size_t)0, array.size());
    CHECK_EQUAL((u32)10, array_moved[0]);
    CHECK_EQUAL((u32)9, array_moved[1]);
    CHECK_EQUAL((u32)5, array_moved[9]);
}

TEST_CASE(copy_and_move_inlined_data)
{
    auto array = Array<u32, 10>(10);
    CHECK_EQUAL((size_t)10, array.size());

    array[0] = 10;
    array[1] = 9;
    array[9] = 5;

    auto array_copy(array);
    CHECK_EQUAL((size_t)10, array.size());
    CHECK_EQUAL((size_t)10, array_copy.size());
    CHECK_EQUAL((u32)10, array[0]);
    CHECK_EQUAL((u32)9, array[1]);
    CHECK_EQUAL((u32)5, array[9]);
    CHECK_EQUAL((u32)10, array_copy[0]);
    CHECK_EQUAL((u32)9, array_copy[1]);
    CHECK_EQUAL((u32)5, array_copy[9]);

    auto array_copy_equals = array;
    CHECK_EQUAL((size_t)10, array.size());
    CHECK_EQUAL((size_t)10, array_copy_equals.size());
    CHECK_EQUAL((u32)10, array[0]);
    CHECK_EQUAL((u32)9, array[1]);
    CHECK_EQUAL((u32)5, array[9]);
    CHECK_EQUAL((u32)10, array_copy_equals[0]);
    CHECK_EQUAL((u32)9, array_copy_equals[1]);
    CHECK_EQUAL((u32)5, array_copy_equals[9]);

    auto array_moved(move(array));
    CHECK_EQUAL((size_t)10, array_moved.size());
    CHECK_EQUAL((size_t)0, array.size());
    CHECK_EQUAL((u32)10, array_moved[0]);
    CHECK_EQUAL((u32)9, array_moved[1]);
    CHECK_EQUAL((u32)5, array_moved[9]);
}

TEST_CASE(first_and_last)
{
    auto array = Array<u32, 10>(10);
    CHECK_EQUAL((size_t)10, array.size());

    array[0] = 10;
    array[1] = 9;
    CHECK_EQUAL((u32)10, array.first());
    CHECK_EQUAL((u32)0, array.last());

    array[9] = 5;
    CHECK_EQUAL((u32)5, array.last());
}

TEST_MAIN(TestArray, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(add);
    ENUMERATE_TEST(shared_pointers);
    ENUMERATE_TEST(big_array);
    ENUMERATE_TEST(inlined_data);
    ENUMERATE_TEST(copy_and_move);
    ENUMERATE_TEST(copy_and_move_inlined_data);
    ENUMERATE_TEST(first_and_last);
})
