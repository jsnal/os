/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Tests/Macros.h>
#include <Universal/ByteBuffer.h>

TEST_CASE(create)
{
    ByteBuffer bb(256);
    CHECK_NONNULL(bb.data());
    CHECK_EQUAL(256, bb.size());

    ByteBuffer bb_zeroed(256, true);
    CHECK_NONNULL(bb_zeroed.data());
    CHECK_EQUAL(256, bb_zeroed.size());

    for (size_t i = 0; i < 256; i++) {
        CHECK_EQUAL(0, bb_zeroed[i]);
    }
}

TEST_CASE(move_and_copy)
{
    ByteBuffer bb(256);
    bb[0] = 0;
    bb[1] = 1;
    bb[2] = 2;
    bb[3] = 3;

    CHECK_EQUAL(0, bb[0]);
    CHECK_EQUAL(1, bb[1]);
    CHECK_EQUAL(2, bb[2]);
    CHECK_EQUAL(3, bb[3]);

    ByteBuffer bb_copy_constructor(bb);
    CHECK_EQUAL(256, bb.size());
    CHECK_EQUAL(256, bb_copy_constructor.size());
    CHECK_EQUAL(0, bb_copy_constructor[0]);
    CHECK_EQUAL(1, bb_copy_constructor[1]);
    CHECK_EQUAL(2, bb_copy_constructor[2]);
    CHECK_EQUAL(3, bb_copy_constructor[3]);

    ByteBuffer bb_copy_assignment = bb;
    CHECK_EQUAL(256, bb.size());
    CHECK_EQUAL(256, bb_copy_assignment.size());
    CHECK_EQUAL(0, bb_copy_assignment[0]);
    CHECK_EQUAL(1, bb_copy_assignment[1]);
    CHECK_EQUAL(2, bb_copy_assignment[2]);
    CHECK_EQUAL(3, bb_copy_assignment[3]);

    ByteBuffer bb_move_constructor(move(bb_copy_constructor));
    CHECK_EQUAL(0, bb_copy_constructor.size());
    CHECK_EQUAL(256, bb_move_constructor.size());
    CHECK_EQUAL(0, bb_move_constructor[0]);
    CHECK_EQUAL(1, bb_move_constructor[1]);
    CHECK_EQUAL(2, bb_move_constructor[2]);
    CHECK_EQUAL(3, bb_move_constructor[3]);

    ByteBuffer bb_move_assignment = move(bb_copy_assignment);
    CHECK_EQUAL(0, bb_copy_assignment.size());
    CHECK_EQUAL(256, bb_move_assignment.size());
    CHECK_EQUAL(0, bb_move_assignment[0]);
    CHECK_EQUAL(1, bb_move_assignment[1]);
    CHECK_EQUAL(2, bb_move_assignment[2]);
    CHECK_EQUAL(3, bb_move_assignment[3]);
}

TEST_CASE(set_and_get)
{
    ByteBuffer bb(256);
    bb[0] = 1;
    CHECK_EQUAL(1, bb[0]);
    bb[10] = 11;
    CHECK_EQUAL(11, bb[10]);
}

TEST_CASE(clear)
{
    ByteBuffer byte_buffer(256);
    byte_buffer[0] = 1;
    byte_buffer[1] = 2;
    byte_buffer[2] = 3;
    byte_buffer[3] = 4;

    CHECK_NONNULL(byte_buffer.data());
    CHECK_EQUAL(256, byte_buffer.size());

    byte_buffer.clear();

    CHECK_NULL(byte_buffer.data());
    CHECK_EQUAL(0, byte_buffer.size());
}

TEST_MAIN(ByteBuffer, [&]() {
    ENUMERATE_TEST(create);
    ENUMERATE_TEST(move_and_copy);
    ENUMERATE_TEST(set_and_get);
    ENUMERATE_TEST(clear);
})
