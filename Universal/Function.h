/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/UniquePtr.h>

namespace Universal {

template<typename ReturnT, typename... ArgumentTs>
class FunctionWrapperBase {
public:
    virtual ~FunctionWrapperBase() { }
    virtual ReturnT invoke(ArgumentTs...) = 0;
};

template<typename RawFunctionT, typename ReturnT, typename... ArgumentTs>
class FunctionWrapper : public FunctionWrapperBase<ReturnT, ArgumentTs...> {
public:
    FunctionWrapper(RawFunctionT&& raw_function)
        : m_raw_function(raw_function)
    {
    }

    ReturnT invoke(ArgumentTs... args) override final
    {
        return m_raw_function(args...);
    }

private:
    RawFunctionT m_raw_function;
};

template<typename>
class Function;

template<typename ReturnT, typename... ArgumentTs>
class Function<ReturnT(ArgumentTs...)> {
public:
    using FunctionT = ReturnT(ArgumentTs...);

    Function() = default;

    Function(std::nullptr_t) { }

    template<typename RawFunctionT>
    Function(RawFunctionT&& raw_function)
        : m_function_wrapper(make_unique_ptr<FunctionWrapper<RawFunctionT, ReturnT, ArgumentTs...>>(move(raw_function)))
    {
    }

    Function& operator=(FunctionT function)
    {
        m_function_wrapper = function;
        return *this;
    }

    ReturnT operator()(ArgumentTs... args)
    {
        return m_function_wrapper->invoke(args...);
    }

private:
    UniquePtr<FunctionWrapperBase<ReturnT, ArgumentTs...>> m_function_wrapper;
};

}

using Universal::Function;
