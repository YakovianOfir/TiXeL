#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | GlobalVersionLock.hpp |
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

#include <atomic>
#include "Infrastructure/NonCopyable.hpp"

namespace Tixel::Txn
{
    class GlobalVersionClock final : public Infra::NonCopyable
	{
		public:
    		using Version = uint64_t;

	    private:
	        explicit GlobalVersionClock();

	    private:
	        static GlobalVersionClock& Instance();

		public:
			static Version Read();
			static Version AddAndFetch();

	    private:
	        std::atomic<Version> _gvc;
    };
}
