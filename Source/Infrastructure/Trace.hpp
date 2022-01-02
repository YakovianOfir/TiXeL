#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Trace.hpp |
//
//   Abstract:
//
//		| Destines a formatted message to the System debugging port,
//		| automatically displayed in the Debugger Command window on
//		| the host computer. Generally, messages sent to this port
//		| might not arrive under Global Flags prohibition.
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

#include <sstream>
#include <iostream>
#include <thread>
#include <string>
#include <fmt\core.h>
#include <fmt\format.h>

namespace Tixel::Infra
{
	extern bool TroubleshootMode;

	enum class TraceLevel
	{
		Information = 0,
		Warning = 1,
		Error = 2,
		Verbose = 3
	};

	void EnableTroubleshootMode();

	std::string TraceLevelToString(const TraceLevel& level);

	template<typename... TArgs>
	void TraceMessage(
		const TraceLevel& level,
		const std::string& function,
		const std::string& format,
		const TArgs&... args) noexcept
	{
		try
		{
			if (!TroubleshootMode && level == TraceLevel::Verbose)
			{
				return;
			}

			std::ostringstream os;

			os << "[TiXeL] ["
			   << TraceLevelToString(level)
			   << "] (T: [" << std::this_thread::get_id() << "]) "
			   << function << "(): "
			   << fmt::format(format, args...)
			   << std::endl;

			std::cout << os.str();
		}
		catch (...)
		{}
	}
}

#ifdef _DEBUG

#define TRACE_INF(__Format, ...) Tixel::Infra::TraceMessage(Tixel::Infra::TraceLevel::Information, __FUNCTION__, __Format, ##__VA_ARGS__)
#define TRACE_ERR(__Format, ...) Tixel::Infra::TraceMessage(Tixel::Infra::TraceLevel::Error, __FUNCTION__, __Format, ##__VA_ARGS__)
#define TRACE_WRN(__Format, ...) Tixel::Infra::TraceMessage(Tixel::Infra::TraceLevel::Warning, __FUNCTION__, __Format, ##__VA_ARGS__)
#define TRACE_VRB(__Format, ...) Tixel::Infra::TraceMessage(Tixel::Infra::TraceLevel::Verbose, __FUNCTION__, __Format, ##__VA_ARGS__)
#define DEBUG_ONLY(__Expression) __Expression

#else

#define TRACE_INF(__Format, ...) ((void)(0))
#define TRACE_ERR(__Format, ...) ((void)(0))
#define TRACE_WRN(__Format, ...) ((void)(0))
#define TRACE_VRB(__Format, ...) ((void)(0))
#define DEBUG_ONLY(__Expression)

#endif
