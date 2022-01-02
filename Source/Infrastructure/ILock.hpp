#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | ILock.hpp |
//
//   Abstract:
//
//		| Interface representing synchronization objects. |
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

#include "NonCopyable.hpp"

namespace Tixel::Infra
{
	class ILock : public NonCopyable
	{
		public:
			virtual bool TryAcquire() = 0;
			virtual void Acquire() = 0;
			virtual void Release() = 0;
			virtual ~ILock() = default;
	};
}