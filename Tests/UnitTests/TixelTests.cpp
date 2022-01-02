#include "TixelTests.hpp"

namespace
{
    class TixelTests : public ::testing::Test
    {
        public:
            virtual ~TixelTests() = default;

        protected:
            virtual void SetUp() override
            {}

            virtual void TearDown() override
            {}

        public:
            static void TxnThreadRoutine(const Tixel::Txn::TransactionRoutine& routine)
            {
                Tixel::Txn::TransactionDispatcher<int, int>::Dispatch(routine);
            }
    };
    TEST_F(TixelTests, TxnListInsertCorrectness)
    {
        // Arrange

        auto txnList = Tixel::Txn::TxnList<int, int>::Create();

        // Act

        Tixel::Txn::TransactionDispatcher<int, int>::Dispatch([&]
        {
            for (auto i = 0; i <= 10; ++i)
            {
                txnList->Insert(i, i);
            }
        });

        // Assert

        ASSERT_EQ(txnList->Sum(), 55);
    }

    TEST_F(TixelTests, TxnListContainsCorrectness)
    {
        // Arrange

        auto txnList = Tixel::Txn::TxnList<int, int>::Create();

        // Act

        Tixel::Txn::TransactionDispatcher<int, int>::Dispatch([&]
        {
            for (auto i = 0; i <= 10; ++i)
            {
                txnList->Insert(i, i);
            }
        });

        // Assert

        for (auto i = 0; i <= 10; ++i)
        {
            ASSERT_TRUE(txnList->Contains(i));
        }
    }

    TEST_F(TixelTests, TxnListGetCorrectness)
    {
        // Arrange

        auto txnList = Tixel::Txn::TxnList<int, int>::Create();

        // Act

        Tixel::Txn::TransactionDispatcher<int, int>::Dispatch([&]
        {
            for (auto i = 0; i <= 10; ++i)
            {
                txnList->Insert(i, i);
            }
        });

        // Assert

        for (auto i = 0; i <= 10; ++i)
        {
            ASSERT_EQ(txnList->Get(i), i);
        }
    }

    TEST_F(TixelTests, TxnListRemoveCorrectness)
    {
        // Arrange

        auto txnList = Tixel::Txn::TxnList<int, int>::Create();

        // Act

        Tixel::Txn::TransactionDispatcher<int, int>::Dispatch([&]
        {
            for (auto i = 0; i <= 10; ++i)
            {
                txnList->Insert(i, i);
                txnList->Remove(i);
            }
        });

        // Assert

        ASSERT_TRUE(txnList->Empty());
    }

    TEST_F(TixelTests, TxnListStressConcurrentTransactions)
    {
        // Arrange

        std::vector<std::thread> threads;

        auto txnList = Tixel::Txn::TxnList<int, int>::Create();

        // Act

        for (auto i = 0; i <= 10; ++i)
        {
            threads.emplace_back(std::thread(TxnThreadRoutine, [&, i]
            {
                txnList->Insert(i, i);
                txnList->Remove(i);
            }));
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        // Assert

        ASSERT_TRUE(txnList->Empty());
    }
}