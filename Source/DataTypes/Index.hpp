#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | Index.hpp |
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

#include <sstream>
#include "Infrastructure/NonCopyable.hpp"
#include "Infrastructure/Trace.hpp"
#include "SkipListMap.hpp"
#include "IndexNode.hpp"
#include "IndexKey.hpp"

namespace Tixel::Adt
{
    template <typename TKey, typename TValue>
    class Index final : public Infra::NonCopyable
    {
        public:
            explicit Index() = default;
            virtual ~Index() = default;

        public:
            using NodeKey = IndexKey<TKey>;
            using Node = IndexNode<TKey, TValue>;
            using NodeGuard = typename Node::Guard;

        public:
            void Insert(NodeGuard& node);
            void Remove(const NodeGuard& node);
            void Insert(NodeKey& key, NodeGuard& node);
            std::pair<NodeKey, NodeGuard> GetPrevious(const TKey& key);

        public:
            void Print();
            bool Empty() const;
            uint64_t Sum() const;
            uint64_t Size() const;

        private:
            mutable sl_map<NodeKey, NodeGuard> _skipListMap;
    };

    template<typename TKey, typename TValue>
    void Index<TKey, TValue>::Insert(NodeGuard& node)
    {
        IndexKey<TKey> indexKey(node->_key);

        Insert(indexKey, node);
    }

    template <typename TKey, typename TValue>
    void Index<TKey, TValue>::Insert(NodeKey& key, NodeGuard& node)
    {
        _skipListMap.insert(std::make_pair(key, node));
    }

    template <typename TKey, typename TValue>
    std::pair<typename Index<TKey, TValue>::NodeKey, typename Index<TKey, TValue>::NodeGuard> Index<TKey, TValue>::GetPrevious(const TKey& key)
    {
        IndexKey<TKey> indexKey(key);

        return *_skipListMap.findPrev(indexKey);
    }

    template<typename TKey, typename TValue>
    void Index<TKey, TValue>::Remove(const NodeGuard& node)
    {
        IndexKey<TKey> indexKey(node->_key);

        _skipListMap.erase(indexKey);
    }

    template <typename TKey, typename TValue>
    bool Index<TKey, TValue>::Empty() const
    {
        return _skipListMap.empty();
    }

    template <typename TKey, typename TValue>
    uint64_t Index<TKey, TValue>::Size() const
    {
        return static_cast<uint64_t>(_skipListMap.size());
    }

    template <typename TKey, typename TValue>
    uint64_t Index<TKey, TValue>::Sum() const
    {
        uint64_t sum = 0;

        for (auto& entry : _skipListMap)
        {
            if (!entry.first.IsSentinel())
            {
                sum += entry.second->_value;
            }
        }

        return sum;
    }

    template <typename TKey, typename TValue>
    void Index<TKey, TValue>::Print()
    {
        std::ostringstream os;

        for (auto& entry : _skipListMap)
        {
            if (!entry.first.IsSentinel())
            {
                os << entry.second->ToString() << " ";
            }
        }

        TRACE_INF("[IDX]: {}", os.str());
    }
}
