/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/ByteBuffer.h>

TEST_CASE(create)
{
    auto byte_buffer = ByteBuffer::create(256);
    CHECK_NONNULL(byte_buffer.ptr());
    CHECK_EQUAL(256ul, byte_buffer.capacity());

    auto byte_buffer_zeroed = ByteBuffer::create_zeroed(256);
    CHECK_NONNULL(byte_buffer.ptr());
    CHECK_EQUAL(256ul, byte_buffer.capacity());

    for (size_t i = 0; i < 256; i++) {
        CHECK_EQUAL((u8)0, byte_buffer.ptr()[i]);
    }
}

TEST_CASE(append)
{
    auto byte_buffer = ByteBuffer::create(256);
    u8 append_buffer[] = { 1, 2, 3, 4 };
    byte_buffer.append(append_buffer, 4);

    CHECK_EQUAL((size_t)4, byte_buffer.size());
    CHECK_EQUAL((u8)1, byte_buffer.ptr()[0]);
    CHECK_EQUAL((u8)2, byte_buffer.ptr()[1]);
    CHECK_EQUAL((u8)3, byte_buffer.ptr()[2]);
    CHECK_EQUAL((u8)4, byte_buffer.ptr()[3]);

    auto byte_buffer_operator = ByteBuffer::create(256);
    byte_buffer_operator += byte_buffer;

    CHECK_EQUAL((size_t)4, byte_buffer_operator.size());
    CHECK_EQUAL((u8)1, byte_buffer_operator.ptr()[0]);
    CHECK_EQUAL((u8)2, byte_buffer_operator.ptr()[1]);
    CHECK_EQUAL((u8)3, byte_buffer_operator.ptr()[2]);
    CHECK_EQUAL((u8)4, byte_buffer_operator.ptr()[3]);
}

TEST_CASE(getters)
{
    auto byte_buffer = ByteBuffer::create(256);
    u8 append_buffer[] = { 1, 2, 3, 4 };
    byte_buffer.append(append_buffer, 4);

    CHECK_NONNULL(byte_buffer.ptr());
    CHECK_EQUAL((size_t)256, byte_buffer.capacity());
    CHECK_EQUAL((size_t)4, byte_buffer.size());
}

TEST_CASE(get)
{
    auto byte_buffer = ByteBuffer::create(256);
    u8 append_buffer[] = { 1, 2, 3, 4 };
    byte_buffer.append(append_buffer, 4);

    CHECK_EQUAL((u8)1, byte_buffer[0]);
    CHECK_EQUAL((u8)2, byte_buffer[1]);
    CHECK_EQUAL((u8)3, byte_buffer[2]);
    CHECK_EQUAL((u8)4, byte_buffer[3]);
}

TEST_CASE(set)
{
    auto byte_buffer = ByteBuffer::create(256);
    CHECK_TRUE(byte_buffer.set(0, 1).is_ok());
    CHECK_EQUAL((u8)1, byte_buffer[0]);

    CHECK_TRUE(byte_buffer.set(10, 11).is_ok());
    CHECK_EQUAL((u8)11, byte_buffer[10]);

    CHECK_TRUE(byte_buffer.set(257, 1).is_error());
}

TEST_CASE(clear)
{
    auto byte_buffer = ByteBuffer::create(256);
    u8 append_buffer[] = { 1, 2, 3, 4 };
    byte_buffer.append(append_buffer, 4);

    CHECK_NONNULL(byte_buffer.ptr());
    CHECK_EQUAL((size_t)4, byte_buffer.size());

    byte_buffer.clear();

    CHECK_NULL(byte_buffer.ptr());
    CHECK_EQUAL((size_t)0, byte_buffer.size());
}

TEST_MAIN(ByteBuffer, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(append);
    ENUMERATE_TEST(getters);
    ENUMERATE_TEST(clear);
    ENUMERATE_TEST(get);
    ENUMERATE_TEST(set);
})
