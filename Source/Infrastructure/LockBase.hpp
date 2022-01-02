#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | LockBase.hpp |
//
//   Abstract:
//
//      | Fundamental abstract class of an ILock object. |
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

#include "ILock.hpp"

namespace Tixel::Infra
{
    class LockBase : public ILock
    {
        public:
            void Acquire() override;
    };
}