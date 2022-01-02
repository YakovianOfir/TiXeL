#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Assert.hpp |
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


#if _DEBUG

#include "Trace.hpp"
#include <cassert>

#define TIXEL_ASSERT(__Expression) \
    \
    if (!(__Expression)) \
    { \
        TRACE_WRN("Assertion failed, leveraging debugger."); \
        assert(__Expression); \
    }

#else

#define TIXEL_ASSERT(__Expression) ((void)(0))

#endif