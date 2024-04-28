/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/ArrayList.h>

template<typename PtrType>
class PtrArrayList : public ArrayList<PtrType> {
public:
    PtrArrayList() { }
};
