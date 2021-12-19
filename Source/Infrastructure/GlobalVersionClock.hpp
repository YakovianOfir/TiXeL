#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | GlobalVersionLock.hpp |
//
//   Abstract:
//
//		|
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
#include "NonCopyable.hpp"

namespace Tixel::Infra
{
    class GlobalVersionClock final : public NonCopyable
	{
	    private:
	        explicit GlobalVersionClock();

	    private:
	        static GlobalVersionClock& Instance();

		public:
			static uint64_t Read();
			static uint64_t AddAndFetch();

	    private:
	        std::atomic<uint64_t> _gvc;
    };
}
