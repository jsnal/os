/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Tests/Universal/IntSharedPtr.h>
#include <Universal/SharedPtr.h>

TEST_CASE(create)
{
    SharedPtr<IntSharedPointer> shared_pointer = adopt(*new IntSharedPointer(1));

    CHECK_EQUAL((int)1, shared_pointer->value());
    shared_pointer->value(2);
    CHECK_EQUAL((int)2, shared_pointer->value());
}

TEST_CASE(multiple_refs)
{
    SharedPtr<IntSharedPointer> shared_pointer = adopt(*new IntSharedPointer(1));
    CHECK_NONNULL(shared_pointer.ptr());
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
    shared_pointer->ref();
    CHECK_EQUAL((u32)2, shared_pointer->ref_count());
    shared_pointer->unref();
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());

    {
        SharedPtr another = *shared_pointer;
        CHECK_EQUAL((u32)2, shared_pointer->ref_count());
    }

    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
}

TEST_CASE(assign_ref)
{
    SharedPtr<IntSharedPointer> shared_pointer = adopt(*new IntSharedPointer(1));
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
    shared_pointer = *shared_pointer;
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
}

TEST_CASE(assign_ptr)
{
    SharedPtr<IntSharedPointer> shared_pointer = adopt(*new IntSharedPointer(1));
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
    shared_pointer = shared_pointer.ptr();
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
}

TEST_CASE(copy_and_move_ref)
{
    SharedPtr<IntSharedPointer> shared_pointer = adopt(*new IntSharedPointer(1));
    CHECK_EQUAL((u32)1, shared_pointer->ref_count());

    {
        auto shared_pointer1 = shared_pointer;
        CHECK_EQUAL((u32)2, shared_pointer->ref_count());

        auto shared_pointer2 = shared_pointer;
        CHECK_EQUAL((u32)3, shared_pointer->ref_count());

        shared_pointer1 = std::move(shared_pointer2);
        CHECK_EQUAL((u32)2, shared_pointer->ref_count());

        SharedPtr<IntSharedPointer> shared_pointer3(std::move(shared_pointer1));
        CHECK_EQUAL((u32)2, shared_pointer->ref_count());

        shared_pointer1 = shared_pointer3;
        CHECK_EQUAL((u32)3, shared_pointer->ref_count());
    }

    CHECK_EQUAL((u32)1, shared_pointer->ref_count());
}

TEST_CASE(swap)
{
    SharedPtr<IntSharedPointer> shared_pointer1 = adopt(*new IntSharedPointer(1));
    SharedPtr<IntSharedPointer> shared_pointer2 = adopt(*new IntSharedPointer(2));
    auto* ptr1 = shared_pointer1.ptr();
    auto* ptr2 = shared_pointer2.ptr();
    swap(ptr1, ptr2);

    CHECK_EQUAL(shared_pointer1.ptr(), ptr2);
    CHECK_EQUAL(shared_pointer2.ptr(), ptr1);
    CHECK_EQUAL((u32)1, shared_pointer1->ref_count());
    CHECK_EQUAL((u32)1, shared_pointer1->ref_count());
}

TEST_MAIN(TestSharedPtr, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(multiple_refs);
    ENUMERATE_TEST(assign_ref);
    ENUMERATE_TEST(assign_ptr);
    ENUMERATE_TEST(copy_and_move_ref);
    ENUMERATE_TEST(swap);
})
