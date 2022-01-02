#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | SingleListOperator.hpp |
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
    class TxnListStorage;

    template <typename TKey, typename TValue>
    class SingleListOperator final : public Adt::IList<TKey, TValue>
    {
        private:
            friend class TxnList<TKey, TValue>;
            using List = TxnList<TKey, TValue>;
            using ListGuard = typename List::Guard;

        public:
            explicit SingleListOperator(Adt::Index<TKey, TValue>& index);

        private:
            friend class Adt::IndexNode<TKey, TValue>;
            using Node = Adt::IndexNode<TKey, TValue>;
            using NodeGuard = typename Node::Guard;
            using NodeTuple = std::tuple<NodeGuard, NodeGuard>;

        private:
            friend class TxnListStorage<TKey, TValue>;
            using ListStorage = TxnListStorage<TKey, TValue>;
            using LocalState = typename ListStorage::TxnLocalState;
            using WriteSetElement = typename ListStorage::WriteSetElement;

        private:
            using Version = GlobalVersionClock::Version;

        public:
            bool Contains(const TKey& key) const override;
            void Insert(const TKey& key, const TValue& value) override;
            std::optional<TValue> Get(const TKey& key) const override;
            std::optional<TValue> Remove(const TKey& key) override;

        private:
            void SetList(const ListGuard& list);

        private:
            NodeTuple TraverseTo(const TKey& key) const;
            NodeGuard GetValidatedPredecessor(const TKey& key) const;

        private:
            ListGuard _list;
            Adt::Index<TKey, TValue>& _index;
    };

    template <typename TKey, typename TValue>
    SingleListOperator<TKey, TValue>::SingleListOperator(Adt::Index<TKey, TValue>& index) : _index(index)
    {}

    template <typename TKey, typename TValue>
    void SingleListOperator<TKey, TValue>::SetList(const ListGuard& list)
    {
        _list = list;
    }

    template <typename TKey, typename TValue>
    bool SingleListOperator<TKey, TValue>::Contains(const TKey& key) const
    {
        TRACE_INF("[Single-Operator]: Contains. (Key: [{}])", key);

        const auto [predecessor, successor] = TraverseTo(key);

        return successor != nullptr && successor->_key == key;
    }

    template <typename TKey, typename TValue>
    std::optional<TValue> SingleListOperator<TKey, TValue>::Get(const TKey& key) const
    {
        TRACE_INF("[Single-Operator]: Get. (Key: [{}])", key);

        const auto [predecessor, successor] = TraverseTo(key);

        if (successor == nullptr || successor->_key != key)
        {
            return std::optional<TValue>();
        }

        return successor->_value;
    }

    template <typename TKey, typename TValue>
    std::optional<TValue> SingleListOperator<TKey, TValue>::Remove(const TKey& key)
    {
        TRACE_INF("[Single-Operator]: Remove. (Key: [{}])", key);

        while (true)
        {
            const auto readVersion = GlobalVersionClock::Read();

            auto [predecessor, successor] = TraverseTo(key);

            if (successor == nullptr || successor->_key != key)
            {
                TRACE_WRN("[Single-Operator]: Remove. (Key: [{}]) (NONE)", key);

                return std::optional<TValue>();
            }

            Infra::Locker predecessorLocker(*predecessor, Infra::Locker::Policy::TryLock);

            if (!predecessorLocker.IsAcquired())
            {
                continue;
            }

            if (predecessor->IsDeleted() || predecessor->_next != successor)
            {
                continue;
            }

            Infra::Locker successorLocker(*successor, Infra::Locker::Policy::TryLock);

            if (!successorLocker.IsAcquired())
            {
                continue;
            }

            auto value = successor->_value;

            predecessor->_next = successor->_next;
            predecessor->SetVersion(readVersion);
            predecessor->SetSingleton(Infra::VersionLock::FlagOption::Enable);

            successor->SetVersion(readVersion);
            successor->SetSingleton(Infra::VersionLock::FlagOption::Enable);
            successor->SetDeleted(Infra::VersionLock::FlagOption::Enable);

            _list->IndexRemove(successor);

            return value;
        }
    }

    template <typename TKey, typename TValue>
    void SingleListOperator<TKey, TValue>::Insert(const TKey& key, const TValue& value)
    {
        TRACE_INF("[Single-Operator]: Insert. (Key: [{}]) (Value: [{}])", key, value);

        while (true)
        {
            const auto readVersion = GlobalVersionClock::Read();

            auto [predecessor, successor] = TraverseTo(key);

            if (successor != nullptr && successor->_key == key)
            {
                Infra::Locker locker(*successor, Infra::Locker::Policy::TryLock);

                if (!locker.IsAcquired())
                {
                    continue;
                }

                successor->_value = value;
                successor->SetSingleton(Infra::VersionLock::FlagOption::Enable);
                successor->SetVersion(readVersion);

                return;
            }

            Infra::Locker locker(*predecessor, Infra::Locker::Policy::TryLock);

            if (!locker.IsAcquired())
            {
                continue;
            }

            if (predecessor->IsDeleted() || predecessor->_next != successor)
            {
                continue;
            }

            auto candidateNode = std::make_shared<Node>(key, value, successor);

            candidateNode->SetSingleton(Infra::VersionLock::FlagOption::Enable);
            candidateNode->SetVersion(readVersion);
            predecessor->_next = candidateNode;

            _list->IndexAdd(candidateNode);

            if (candidateNode->IsDeleted())
            {
                _list->IndexRemove(candidateNode);
            }

            return;
        }
    }

    template <typename TKey, typename TValue>
    typename SingleListOperator<TKey, TValue>::NodeTuple SingleListOperator<TKey, TValue>::TraverseTo(const TKey& key) const
    {
        while (true)
        {
            auto predecessor = GetValidatedPredecessor(key);

            while (true)
            {
                if (predecessor->IsLocked())
                {
                    break;
                }

                const auto& successor = predecessor->_next;

                if (successor != nullptr && successor->IsLockedOrDeleted())
                {
                    break;
                }

                if (successor == nullptr || successor->_key >= key)
                {
                    return { predecessor, successor };
                }

                predecessor = successor;
            }
        }
    }

    template <typename TKey, typename TValue>
    typename SingleListOperator<TKey, TValue>::NodeGuard SingleListOperator<TKey, TValue>::GetValidatedPredecessor(const TKey& key) const
    {
        auto candidateKey = key;

        while (true)
        {
            const auto& [predecessorKey, predecessor] = _index.GetPrevious(candidateKey);

            if (!predecessor->IsLockedOrDeleted() || !predecessorKey.IsSentinel())
            {
                return predecessor;
            }

            candidateKey = predecessorKey.Key();
        }
    }
}
