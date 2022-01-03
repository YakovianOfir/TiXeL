#include "Infrastructure/TixellInfra.hpp"
#include "Transactions/TixelTxn.hpp"
#include "DataTypes/TixelAdt.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <thread>

enum ReturnValues : int32_t
{
    RvSuccess = 0,
    RvInvalidCmdLine = -1,
    RvGeneralFault = -2,
    RvCriticalFault = -3
};

void ThreadRoutine(const Tixel::Txn::TransactionRoutine& routine)
{
    Tixel::Txn::TransactionDispatcher<int, int>::Dispatch(routine);
}

int wmain(const int argc, wchar_t const* const argv[])
{
    try
    {
        Tixel::Infra::CHECK_CMDLINE(argc == 3, fmt::format("Usage: {} <Thread-Count> <Troubleshoot-Mode 0|1>", argv[0]));

        if (std::stoi(argv[2]) == 1)
        {
            Tixel::Infra::EnableTroubleshootMode();
        }

        const auto threadCount = std::stoi(argv[1]);

        TRACE_INF("Starting workload. (Threads: {})", threadCount);

        auto txnList = Tixel::Txn::TxnList<int, int>::Create();

        std::vector<std::thread> threads;

        for (auto i = 0; i < threadCount; ++i)
        {
            threads.emplace_back(std::thread(ThreadRoutine, [&, i]
            {
                txnList->Insert(2 * i, 2 * i);
                txnList->Remove(2 * i + 1);
            }));

            threads.emplace_back(std::thread(ThreadRoutine, [&, i]
            {
                txnList->Insert(2 * i + 1, 2 * i + 1);
                txnList->Remove(2 * i);
            }));
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        txnList->Print();
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Invalid command line: [" << e.what() << "]" << std::endl;
        return RvInvalidCmdLine;
    }
    catch (const std::exception& e)
    {
        std::cout << "General exception: [" << e.what() << "]" << std::endl;
        return RvGeneralFault;
    }
    catch (...)
    {
        std::cout << "Critical exception. Aborting" << std::endl;
        return RvCriticalFault;
    }
}