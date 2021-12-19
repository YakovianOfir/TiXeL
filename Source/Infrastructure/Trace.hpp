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

#ifdef _DEBUG

#include <iostream>
#include <thread>
#include <string>
#include <fmt\core.h>

namespace Tixel::Infra
{
	template<typename... TArgs>
	void TraceMessage(
		const std::string& level,
		const std::string& function,
		const std::string& format,
		const TArgs&... args)
	{
		try
		{
			std::cout << "[TiXeL] [" << level << "] (T: ["
				<< std::this_thread::get_id() << "]) "
				<< function << "(): "
				<< fmt::format(format, args...)
				<< std::endl;
		}
		catch (...)
		{}
	}
}

#define TRACE_INF(__Format, ...) Tixel::Infra::TraceMessage("INF", __FUNCTION__, __Format, ##__VA_ARGS__)
#define TRACE_ERR(__Format, ...) Tixel::Infra::TraceMessage("ERR", __FUNCTION__, __Format, ##__VA_ARGS__)
#define TRACE_WRN(__Format, ...) Tixel::Infra::TraceMessage("WRN", __FUNCTION__, __Format, ##__VA_ARGS__)
#define TRACE_VRB(__Format, ...) Tixel::Infra::TraceMessage("VRB", __FUNCTION__, __Format, ##__VA_ARGS__)
#define DEBUG_ONLY(__Expression) __Expression

#else

#define TRACE_INF(__Format, ...) ((void)(0))
#define TRACE_ERR(__Format, ...) ((void)(0))
#define TRACE_WRN(__Format, ...) ((void)(0))
#define TRACE_VRB(__Format, ...) ((void)(0))
#define DEBUG_ONLY(__Expression)

#endif
