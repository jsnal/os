/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef KERNEL
#    include <Kernel/Stdlib.h>
#    include <Kernel/kmalloc.h>
#else
#    include <stdlib.h>
#    include <string.h>
#endif

template<typename T>
struct RemoveReference {
    typedef T Type;
};
template<typename T>
struct RemoveReference<T&> {
    typedef T Type;
};
template<typename T>
struct RemoveReference<T&&> {
    typedef T Type;
};

template<typename T>
inline constexpr T&& forward(typename RemoveReference<T>::Type& param)
{
    return static_cast<T&&>(param);
}

template<typename T>
inline constexpr T&& forward(typename RemoveReference<T>::Type&& param)
{
    return static_cast<T&&>(param);
}

template<typename T>
T&& move(T& arg)
{
    return static_cast<T&&>(arg);
}

template<typename T, typename U>
inline void swap(T& a, U& b)
{
    U tmp = move((U&)a);
    a = (T &&) move(b);
    b = move(tmp);
}

template<typename T, typename U = T>
inline constexpr T exchange(T& slot, U&& value)
{
    T old = move(slot);
    slot = forward<T>(value);
    return old;
}
