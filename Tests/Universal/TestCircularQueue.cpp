/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/CircularQueue.h>

TEST_CASE(create)
{
    auto queue = CircularQueue<u32>(10);
    CHECK_EQUAL((size_t)0, queue.size());
    CHECK_EQUAL((size_t)10, queue.capacity());
}

TEST_CASE(enqueue_dequeue)
{
    auto queue = CircularQueue<u32>(4);
    CHECK_EQUAL((size_t)0, queue.size());
    CHECK_EQUAL((size_t)4, queue.capacity());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    CHECK_EQUAL((u32)1, queue.dequeue());
    CHECK_EQUAL((u32)2, queue.dequeue());
    CHECK_EQUAL((u32)3, queue.dequeue());
    CHECK_EQUAL((u32)4, queue.dequeue());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    CHECK_EQUAL((u32)1, queue.dequeue());
    CHECK_EQUAL((u32)2, queue.dequeue());

    queue.enqueue(4);

    CHECK_EQUAL((u32)3, queue.dequeue());
    CHECK_EQUAL((u32)4, queue.dequeue());
}

TEST_CASE(front_back)
{
    auto queue = CircularQueue<u32>(4);
    CHECK_EQUAL((size_t)0, queue.size());
    CHECK_EQUAL((size_t)4, queue.capacity());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    CHECK_EQUAL((u32)1, queue.front());
    CHECK_EQUAL((u32)4, queue.back());

    CHECK_EQUAL((u32)1, queue.dequeue());
    CHECK_EQUAL((u32)2, queue.front());
    CHECK_EQUAL((u32)4, queue.back());

    CHECK_EQUAL((u32)2, queue.dequeue());
    CHECK_EQUAL((u32)3, queue.front());
    CHECK_EQUAL((u32)4, queue.back());

    CHECK_EQUAL((u32)3, queue.dequeue());
    CHECK_EQUAL((u32)4, queue.front());
    CHECK_EQUAL((u32)4, queue.back());

    CHECK_EQUAL((u32)4, queue.dequeue());
    CHECK_TRUE(queue.is_empty());
}

TEST_CASE(move)
{
    auto queue = CircularQueue<u32>(4);
    CHECK_EQUAL((size_t)0, queue.size());
    CHECK_EQUAL((size_t)4, queue.capacity());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    auto queue_construct = CircularQueue<u32>(move(queue));
    CHECK_FALSE(queue_construct.is_empty());
    CHECK_TRUE(queue.is_empty());
    CHECK_EQUAL((size_t)4, queue_construct.size());
    CHECK_EQUAL((size_t)4, queue_construct.capacity());

    CHECK_EQUAL((u32)1, queue_construct.front());
    CHECK_EQUAL((u32)4, queue_construct.back());

    auto queue_equals = CircularQueue<u32>(queue_construct.size());
    CHECK_TRUE(queue_equals.is_empty());
    CHECK_EQUAL((size_t)4, queue_equals.capacity());

    queue_equals = move(queue_construct);

    CHECK_TRUE(queue.is_empty());
    CHECK_TRUE(queue_construct.is_empty());
    CHECK_EQUAL((u32)1, queue_equals.front());
    CHECK_EQUAL((u32)4, queue_equals.back());
}

TEST_MAIN(TestCircularQueue, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(enqueue_dequeue);
    ENUMERATE_TEST(front_back);
    ENUMERATE_TEST(move);
})
