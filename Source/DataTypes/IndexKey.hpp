#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | IndexKey.hpp |
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

#include <optional>

namespace Tixel::Adt
{
    template <typename TKey>
    class IndexKey
    {
        public:
            explicit IndexKey() = default;

            explicit IndexKey(const std::optional<TKey>& key) : _key(key)
            {}

            explicit IndexKey(const TKey& key)
            {
                _key = key;
            }

            TKey Key() const
            {
                return _key.value();
            }

            bool IsSentinel() const
            {
                return !_key.has_value();
            }

        private:
            std::optional<TKey> _key;
    };

    template <typename TKey>
    bool operator<(const IndexKey<TKey>& lhs, const IndexKey<TKey>& rhs)
    {
        if (lhs.IsSentinel())
        {
            return true;
        }

        if (rhs.IsSentinel())
        {
            return false;
        }

        return lhs.Key() < rhs.Key();
    }

    template <typename TKey>
    bool operator>(const IndexKey<TKey>& lhs, const IndexKey<TKey>& rhs)
    {
        if (lhs.IsSentinel())
        {
            return false;
        }

        if (rhs.IsSentinel())
        {
            return true;
        }

        return lhs.Key() > rhs.Key();
    }

    template <typename TKey>
    bool operator==(const IndexKey<TKey>& lhs, const IndexKey<TKey>& rhs)
    {
        if (lhs.IsSentinel() && rhs.IsSentinel())
        {
            return true;
        }

        if (lhs.IsSentinel() && !rhs.IsSentinel())
        {
            return false;
        }

        if (!lhs.IsSentinel() && rhs.IsSentinel())
        {
            return false;
        }

        return lhs.Key() == rhs.Key();
    }

    template <typename TKey>
    bool operator!=(const IndexKey<TKey>& lhs, const IndexKey<TKey>& rhs)
    {
        return !operator==(lhs, rhs);
    }

    template <typename TKey>
    bool operator>=(const IndexKey<TKey>& lhs, const IndexKey<TKey>& rhs)
    {
        return !operator<(lhs, rhs);
    }

    template <typename TKey>
    bool operator<=(const IndexKey<TKey>& lhs, const IndexKey<TKey>& rhs)
    {
        return !operator>(lhs, rhs);
    }
}