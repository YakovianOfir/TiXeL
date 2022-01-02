#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | IList.hpp |
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

#include "Infrastructure/NonCopyable.hpp"

namespace Tixel::Adt
{
    template <typename TKey, typename TValue>
    class IList : public Infra::NonCopyable
    {
        public:
            virtual ~IList() = default;

        public:
            virtual std::optional<TValue> Get(const TKey& key) const = 0;
            virtual std::optional<TValue> Remove(const TKey& key) = 0;
            virtual void Insert(const TKey& key, const TValue& value) = 0;
            virtual bool Contains(const TKey& key) const = 0;
    };
}
