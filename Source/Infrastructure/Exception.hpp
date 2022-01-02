#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Exception.hpp |
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

#include <exception>
#include <stdexcept>
#include <string>

namespace Tixel::Infra
{
	class CmdLineException final : public std::invalid_argument
	{
		public:
			explicit CmdLineException(const std::string& message);
	};

	class TransactionException final : public std::runtime_error
	{
		public:
			explicit TransactionException();
			explicit TransactionException(const std::string& message);
	};

	void ROLLBACK_TXN(bool condition, const std::string& message);
	void CHECK_CMDLINE(bool condition, const std::string& message);
	void ROLLBACK_TXN(bool condition);
}
