#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | TixelTests.hpp |
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

#include "gtest/gtest.h"
#include "Infrastructure/TixellInfra.hpp"
#include "Transactions/TixelTxn.hpp"
#include "DataTypes/TixelAdt.hpp"

namespace TixelUnitTests
{
    using namespace Tixel::Infra;
    using namespace Tixel::Adt;
    using namespace Tixel::Txn;
    using namespace ::testing;
}