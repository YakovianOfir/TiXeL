#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Locker.hpp |
//
//   Abstract:
//
//		| RAII over a possession scope of synchronization objects. |
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
#include "LockBase.hpp"

namespace Tixel::Infra
{
	class Locker final : public NonCopyable
	{
		public:
			enum class Policy
			{
				TryLock,
				ForceLock
			};

		public:
			explicit Locker(LockBase& lockObject);
			explicit Locker(LockBase& lockObject, const Policy& lockPolicy);
			virtual ~Locker();

		public:
			bool IsAcquired() const;

		private:
			LockBase& _lockObject;
			bool _isLocked;
	};
}
