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

TEST_MAIN(TestArray, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(add);
    ENUMERATE_TEST(shared_pointers);
})
