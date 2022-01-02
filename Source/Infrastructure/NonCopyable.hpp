#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | NonCopyable.hpp |
//
//   Abstract:
//
//      | Explicitly disables the creation of default, copy, move and
//      | assignment constructors, using simple C++11 dialects.
//
//   Author:
//
//      | Ofir Yakovian (@ofirkov) |
//
//   Environment:
//
//      | User Mode |
//
/////////////////////////////////////////////////////////////////

namespace Tixel::Infra
{
    class NonCopyable
    {
        public:
            explicit NonCopyable() = default;
            virtual ~NonCopyable() = default;

        public:
            explicit NonCopyable(NonCopyable&&) = delete;
            explicit NonCopyable(const NonCopyable&) = delete;

        public:
            NonCopyable& operator=(const NonCopyable&) = delete;
            NonCopyable& operator=(NonCopyable&&) = delete;
    };
}
