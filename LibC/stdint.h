/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sys/cdefs.h>

__BEGIN_DECLS

typedef __UINT64_TYPE__ uint64_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT8_TYPE__ uint8_t;

typedef __INT64_TYPE__ int64_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT8_TYPE__ int8_t;

typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;

typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;

typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__ intptr_t;

#define UINT8_MAX (__UINT8_MAX__)
#define UINT16_MAX (__UINT16_MAX__)
#define UINT32_MAX (__UINT32_MAX__)
#define UINT64_MAX (__UINT64_MAX__)

#define INT8_MAX (__INT8_MAX__)
#define INT16_MAX (__INT16_MAX__)
#define INT32_MAX (__INT32_MAX__)
#define INT64_MAX (__INT64_MAX__)

#define UINT_FAST8_MAX (__UINT_FAST8_MAX__)
#define UINT_FAST16_MAX (__UINT_FAST16_MAX__)
#define UINT_FAST32_MAX (__UINT_FAST32_MAX__)
#define UINT_FAST64_MAX (__UINT_FAST64_MAX__)

#define INT_FAST8_MAX (__INT_FAST8_MAX__)
#define INT_FAST16_MAX (__INT_FAST16_MAX__)
#define INT_FAST32_MAX (__INT_FAST32_MAX__)
#define INT_FAST64_MAX (__INT_FAST64_MAX__)

#define UINT_LEAST8_MAX (__UINT_LEAST8_MAX__)
#define UINT_LEAST16_MAX (__UINT_LEAST16_MAX__)
#define UINT_LEAST32_MAX (__UINT_LEAST32_MAX__)
#define UINT_LEAST64_MAX (__UINT_LEAST64_MAX__)

#define INT_LEAST8_MAX (__INT_LEAST8_MAX__)
#define INT_LEAST16_MAX (__INT_LEAST16_MAX__)
#define INT_LEAST32_MAX (__INT_LEAST32_MAX__)
#define INT_LEAST64_MAX (__INT_LEAST64_MAX__)

#define INTPTR_MAX (__INTPTR_MAX__)
#define UINTPTR_MAX (__UINTPTR_MAX__)

__END_DECLS
