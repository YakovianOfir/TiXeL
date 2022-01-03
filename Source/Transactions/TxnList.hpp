#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | TxnList.hpp |
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

#include "TxnListStorage.hpp"
#include "DataTypes/IList.hpp"

namespace Tixel::Txn
{
    template <typename TKey, typename TValue>
    class TxnListOperator;

    template <typename TKey, typename TValue>
    class SingleListOperator;

    template <typename TKey, typename TValue>
    class TxnListStorage;

    template <typename TKey, typename TValue>
    class TxnList final : public Adt::IList<TKey, TValue>, public std::enable_shared_from_this<TxnList<TKey, TValue>>
    {
        public:
            using Guard = std::shared_ptr<TxnList>;

        public:
            static Guard Create();

        public:
            explicit TxnList();
            void ImbueOperators();

        private:
            friend class Adt::IndexKey<TKey>;
            using NodeKey = Adt::IndexKey<TKey>;

        private:
            friend class Adt::IndexNode<TKey, TValue>;
            using Node = Adt::IndexNode<TKey, TValue>;
            using NodeGuard = typename Node::Guard;

        private:
            friend class TxnListStorage<TKey, TValue>;
            using ListStorage = TxnListStorage<TKey, TValue>;
            using LocalState = typename ListStorage::TxnLocalState;
            using WriteSetElement = typename ListStorage::WriteSetElement;

        private:
            friend class TxnListOperator<TKey, TValue>;
            friend class SingleListOperator<TKey, TValue>;

        public:
            bool Contains(const TKey& key) const override;
            void Insert(const TKey& key, const TValue& value) override;
            std::optional<TValue> Get(const TKey& key) const override;
            std::optional<TValue> Remove(const TKey& key) override;

        private:
            void IndexAdd(NodeGuard& node) const;
            void IndexAdd(std::vector<NodeGuard>& nodes) const;
            void IndexRemove(const NodeGuard& node) const;
            void IndexRemove(const std::unordered_set<NodeGuard>& nodes) const;

        public:
            void Print() const;
            bool Empty() const;
            uint64_t Sum() const;
            uint64_t Size() const;

        private:
            mutable Adt::Index<TKey, TValue> _index;
            mutable TxnListOperator<TKey, TValue> _txnOperator;
            mutable SingleListOperator<TKey, TValue> _singleOperator;
    };

    template <typename TKey, typename TValue>
    typename TxnList<TKey, TValue>::Guard TxnList<TKey, TValue>::Create()
    {
        auto txnList = std::make_shared<TxnList<TKey, TValue>>();

        txnList->ImbueOperators();

        return txnList;
    }

    template <typename TKey, typename TValue>
    TxnList<TKey, TValue>::TxnList() : _txnOperator(_index), _singleOperator(_index)
    {
        NodeGuard sentinel = std::make_shared<Node>();

        sentinel->SetVersion(GlobalVersionClock::Read());
        sentinel->SetSingleton(Infra::VersionLock::FlagOption::Disable);

        NodeKey sentinelKey(std::nullopt);

        _index.Insert(sentinelKey, sentinel);
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::ImbueOperators()
    {
        _txnOperator.SetList(__super::shared_from_this());
        _singleOperator.SetList(__super::shared_from_this());
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::Print() const
    {
        _index.Print();
    }

    template <typename TKey, typename TValue>
    bool TxnList<TKey, TValue>::Empty() const
    {
        return _index.Empty() || Size() == 1;
    }

    template <typename TKey, typename TValue>
    uint64_t TxnList<TKey, TValue>::Sum() const
    {
        return _index.Sum();
    }

    template <typename TKey, typename TValue>
    uint64_t TxnList<TKey, TValue>::Size() const
    {
        return _index.Size();
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::IndexAdd(NodeGuard& node) const
    {
        _index.Insert(node);
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::IndexRemove(const NodeGuard& node) const
    {
        _index.Remove(node);
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::IndexAdd(std::vector<NodeGuard>& nodes) const
    {
        for (uint64_t i = 0; i != nodes.size(); ++i)
        {
            IndexAdd(nodes[i]);
        }
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::IndexRemove(const std::unordered_set<NodeGuard>& nodes) const
    {
        for (const auto& node : nodes)
        {
            IndexRemove(node);
        }
    }

    template <typename TKey, typename TValue>
    bool TxnList<TKey, TValue>::Contains(const TKey& key) const
    {
        auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

        return localStorage.LocalTxnIsActive() ? _txnOperator.Contains(key) : _singleOperator.Contains(key);
    }

    template <typename TKey, typename TValue>
    std::optional<TValue> TxnList<TKey, TValue>::Get(const TKey& key) const
    {
        auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

        return localStorage.LocalTxnIsActive() ? _txnOperator.Get(key) : _singleOperator.Get(key);
    }

    template <typename TKey, typename TValue>
    void TxnList<TKey, TValue>::Insert(const TKey& key, const TValue& value)
    {
        auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

        return localStorage.LocalTxnIsActive() ? _txnOperator.Insert(key, value) : _singleOperator.Insert(key, value);
    }

    template <typename TKey, typename TValue>
    std::optional<TValue> TxnList<TKey, TValue>::Remove(const TKey& key)
    {
        auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

        return localStorage.LocalTxnIsActive() ? _txnOperator.Remove(key) : _singleOperator.Remove(key);
    }
}


