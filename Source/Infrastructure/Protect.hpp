#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Protect.hpp |
//
//   Abstract:
//
//		| This module provides a standard facility to ensure that
//		| destructor code doesn't throw any exceptions.
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

#include "Trace.hpp"
#include "Assert.hpp"

#define PROTECT_DESTRUCTOR(__DestructionRoutine) \
	\
	try \
	{ \
		__DestructionRoutine; \
	} \
	catch (const std::exception& DEBUG_ONLY(e)) \
	{ \
		TRACE_ERR("Destruction (general) fault. (Error: [{}}])", e.what()); \
		TIXEL_ASSERT(false); \
	} \
	catch (...) \
	{ \
		TRACE_ERR("Destruction (critical) fault."); \
		TIXEL_ASSERT(false); \
	}
