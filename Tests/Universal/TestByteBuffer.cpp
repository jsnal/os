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

    CHECK_EQUAL((u8)1, byte_buffer.ptr()[0]);
    CHECK_EQUAL((u8)2, byte_buffer.ptr()[1]);
    CHECK_EQUAL((u8)3, byte_buffer.ptr()[2]);
    CHECK_EQUAL((u8)4, byte_buffer.ptr()[3]);
}

TEST_MAIN(TestByteBuffer, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(append);
})
