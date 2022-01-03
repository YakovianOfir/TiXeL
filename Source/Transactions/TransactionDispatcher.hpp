#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | TransactionDispatcher.hpp |
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

#include "TransactionRoutine.hpp"
#include "TxnListTransaction.hpp"

namespace Tixel::Txn
{
    template <typename TKey, typename TValue>
    class TxnListTransaction;

    template <typename TKey, typename TValue>
    class TransactionDispatcher final : public Infra::NonCopyable
    {
        public:
            explicit TransactionDispatcher() = delete;

        private:
            friend class TxnListTransaction<TKey, TValue>;
            using Transaction = TxnListTransaction<TKey, TValue>;

        public:
            static void Dispatch(const TransactionRoutine& txnRoutine);
            static bool TryDispatch(const TransactionRoutine& txnRoutine);
    };

    template <typename TKey, typename TValue>
    void TransactionDispatcher<TKey, TValue>::Dispatch(const TransactionRoutine& txnRoutine)
    {
        while (!TryDispatch(txnRoutine)) {}
    }

    template <typename TKey, typename TValue>
    bool TransactionDispatcher<TKey, TValue>::TryDispatch(const TransactionRoutine& txnRoutine)
    {
        try
        {
            TRACE_INF("[Dispatcher]: Begin.");

            Transaction::Begin();

            TRACE_VRB("[Dispatcher]: Execute.");

            txnRoutine();

            TRACE_INF("[Dispatcher]: Commit.");

            Transaction::Commit();

            TRACE_INF("[Dispatcher]: Done. (OK)");

            return true;
        }
        catch (const Infra::TransactionException&)
        {
            TRACE_INF("[Dispatcher]: Rollback.");

            Transaction::Rollback();

            TRACE_INF("[Dispatcher]: Done. (ABORT)");

            return false;
        }
        catch (...)
        {
            TRACE_ERR("[Dispatcher]: Rollback.");

            Transaction::Rollback();

            TRACE_ERR("[Dispatcher]: Done. (FAULT)");

            throw;
        }
    }
}
