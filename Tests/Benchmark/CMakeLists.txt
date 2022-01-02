cmake_minimum_required(VERSION 3.15)

project(TiXeL)

set(CMAKE_CXX_STANDARD 17)

set(TixelInfraSources
	Source/Infrastructure/Assert.hpp
	Source/Infrastructure/Exception.cpp
	Source/Infrastructure/Exception.hpp
	Source/Infrastructure/Flags.hpp
	Source/Infrastructure/ILock.hpp
	Source/Infrastructure/LockBase.cpp
	Source/Infrastructure/LockBase.hpp
	Source/Infrastructure/Locker.cpp
	Source/Infrastructure/Locker.hpp
	Source/Infrastructure/NonCopyable.hpp
	Source/Infrastructure/Packages.config
	Source/Infrastructure/Protect.hpp
	Source/Infrastructure/TixellInfra.hpp
	Source/Infrastructure/Trace.cpp
	Source/Infrastructure/Trace.hpp
	Source/Infrastructure/VersionLock.cpp
	Source/Infrastructure/VersionLock.hpp
)

set(TixelAdtSources
	Source/DataTypes/IList.hpp
	Source/DataTypes/Index.hpp
	Source/DataTypes/IndexKey.hpp
	Source/DataTypes/IndexNode.hpp
	Source/DataTypes/SkipList.cpp
	Source/DataTypes/SkipList.hpp
	Source/DataTypes/SkipListMap.hpp
	Source/DataTypes/TixelAdt.hpp
)

set(TixelTxnSources
	Source/Transactions/GlobalVersionClock.cpp
	Source/Transactions/GlobalVersionClock.hpp
	Source/Transactions/SingleListOperator.hpp
	Source/Transactions/TixelTxn.hpp
	Source/Transactions/TransactionDefinitions.hpp
	Source/Transactions/TransactionDispatcher.hpp
	Source/Transactions/TransactionRoutine.hpp
	Source/Transactions/TxnList.hpp
	Source/Transactions/TxnListOperator.hpp
	Source/Transactions/TxnListStorage.hpp
	Source/Transactions/TxnListTransaction.hpp
)

add_library(TixelInfra STATIC ${TixelInfraSources})
add_library(TixelAdt STATIC ${TixelAdtSources})
add_library(TixelTxn STATIC ${TixelTxnSources})

add_executable(TixelConsole
        ${TixelInfraSources}
        ${TixelAdtSources}
        ${TixelTxnSources}
        Source/Console/TixelMain.cpp)

add_executable(TixelBench
        ${TixelInfraSources}
        ${TixelAdtSources}
        ${TixelTxnSources}
        Tests/Benchmark/IntSet.cpp
		Tests/Benchmark/IntSet.hpp
		Tests/Benchmark/Main.cpp
		Tests/Benchmark/TixelList.cpp
		Tests/Benchmark/TixelList.hpp)

add_executable(TixelUnitTests
        ${TixelInfraSources}
        ${TixelAdtSources}
        ${TixelTxnSources}
		Tests/UnitTests/TixelTests.cpp
		Tests/UnitTests/TixelTests.hpp)
		
TARGET_LINK_LIBRARIES(TixelConsole TixelInfra TixelAdt TixelTxn)
TARGET_LINK_LIBRARIES(TixelBench TixelInfra TixelAdt TixelTxn)
TARGET_LINK_LIBRARIES(TixelUnitTests TixelInfra TixelAdt TixelTxn)

set_target_properties(
    TixelConsole 
    TixelBench
	TixelUnitTests
    PROPERTIES 
        VS_PACKAGE_REFERENCES "fmt.1.2"
)
