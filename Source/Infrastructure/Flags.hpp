#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Flags.hpp |
//
//   Author:
//
//      | Ofir Yakovian (@ofirkov) |
//
//   Environment:
//
//		| User Mode |
//
/////////////////////////////////////////////////////////////////

namespace Tixel::Infra
{
	template <typename TValue>
    constexpr bool IsFlagSet(TValue value, TValue flagValue)
    {
        return (value & flagValue) != 0;
    }

    template <typename TValue>
    constexpr void SetFlag(TValue& value, TValue flagValue)
    {
        value |= flagValue;
    }

    template <typename TValue>
    constexpr void ClearFlag(TValue& value, TValue flagValue)
    {
        value &= ~flagValue;
    }
}
