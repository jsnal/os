/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/CircularQueue.h>

TEST_CASE(create)
{
    CircularQueue<u32, 10> queue;
    CHECK_EQUAL(0, queue.size());
    CHECK_EQUAL(10, queue.capacity());
}

TEST_CASE(enqueue_dequeue)
{
    CircularQueue<u32, 4> queue;
    CHECK_EQUAL(0, queue.size());
    CHECK_EQUAL(4, queue.capacity());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    CHECK_EQUAL(1, queue.dequeue());
    CHECK_EQUAL(2, queue.dequeue());
    CHECK_EQUAL(3, queue.dequeue());
    CHECK_EQUAL(4, queue.dequeue());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    CHECK_EQUAL(1, queue.dequeue());
    CHECK_EQUAL(2, queue.dequeue());

    queue.enqueue(4);

    CHECK_EQUAL(3, queue.dequeue());
    CHECK_EQUAL(4, queue.dequeue());
}

TEST_CASE(front_back)
{
    CircularQueue<u32, 4> queue;
    CHECK_EQUAL(0, queue.size());
    CHECK_EQUAL(4, queue.capacity());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    CHECK_EQUAL(1, queue.front());
    CHECK_EQUAL(4, queue.back());

    CHECK_EQUAL(1, queue.dequeue());
    CHECK_EQUAL(2, queue.front());
    CHECK_EQUAL(4, queue.back());

    CHECK_EQUAL(2, queue.dequeue());
    CHECK_EQUAL(3, queue.front());
    CHECK_EQUAL(4, queue.back());

    CHECK_EQUAL(3, queue.dequeue());
    CHECK_EQUAL(4, queue.front());
    CHECK_EQUAL(4, queue.back());

    CHECK_EQUAL(4, queue.dequeue());
    CHECK_TRUE(queue.is_empty());
}

TEST_CASE(copy_and_move)
{
    CircularQueue<u32, 4> queue;
    CHECK_EQUAL(0, queue.size());
    CHECK_EQUAL(4, queue.capacity());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    CircularQueue<u32, 4> queue_copy_constructor(queue);
    CHECK_EQUAL(4, queue.size());
    CHECK_EQUAL(4, queue.capacity());
    CHECK_EQUAL(4, queue_copy_constructor.size());
    CHECK_EQUAL(4, queue_copy_constructor.capacity());
    CHECK_EQUAL(1, queue_copy_constructor.dequeue());
    CHECK_EQUAL(2, queue_copy_constructor.dequeue());
    CHECK_EQUAL(3, queue_copy_constructor.dequeue());
    CHECK_EQUAL(4, queue_copy_constructor.dequeue());
    CHECK_TRUE(queue_copy_constructor.is_empty());

    CircularQueue<u32, 4> queue_copy_assignment = queue;
    CHECK_EQUAL(4, queue.size());
    CHECK_EQUAL(4, queue.capacity());
    CHECK_EQUAL(4, queue_copy_assignment.size());
    CHECK_EQUAL(4, queue_copy_assignment.capacity());
    CHECK_EQUAL(1, queue_copy_assignment.dequeue());
    CHECK_EQUAL(2, queue_copy_assignment.dequeue());
    CHECK_EQUAL(3, queue_copy_assignment.dequeue());
    CHECK_EQUAL(4, queue_copy_assignment.dequeue());
    CHECK_TRUE(queue_copy_assignment.is_empty());

    CircularQueue<u32, 4> queue_copy1(queue);
    CircularQueue<u32, 4> queue_move_constructor(move(queue_copy1));
    CHECK_EQUAL(0, queue_copy1.size());
    CHECK_EQUAL(4, queue_move_constructor.size());
    CHECK_EQUAL(4, queue_move_constructor.capacity());
    CHECK_EQUAL(1, queue_move_constructor.dequeue());
    CHECK_EQUAL(2, queue_move_constructor.dequeue());
    CHECK_EQUAL(3, queue_move_constructor.dequeue());
    CHECK_EQUAL(4, queue_move_constructor.dequeue());
    CHECK_TRUE(queue_move_constructor.is_empty());

    CircularQueue<u32, 4> queue_copy2(queue);
    CircularQueue<u32, 4> queue_move_assignment = move(queue_copy2);
    CHECK_EQUAL(0, queue_copy2.size());
    CHECK_EQUAL(4, queue_move_assignment.size());
    CHECK_EQUAL(4, queue_move_assignment.capacity());
    CHECK_EQUAL(1, queue_move_assignment.dequeue());
    CHECK_EQUAL(2, queue_move_assignment.dequeue());
    CHECK_EQUAL(3, queue_move_assignment.dequeue());
    CHECK_EQUAL(4, queue_move_assignment.dequeue());
    CHECK_TRUE(queue_move_assignment.is_empty());
}

TEST_MAIN(TestCircularQueue, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(enqueue_dequeue);
    ENUMERATE_TEST(front_back);
    ENUMERATE_TEST(copy_and_move);
})
