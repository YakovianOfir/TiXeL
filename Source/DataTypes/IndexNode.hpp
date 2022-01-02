#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | IndexNode.hpp |
//
//   Author:
//
//      | Ofir Yakovian (@ofirkov) |
//
//   Environment:
//
//		| User Mode |
//
/////////////////////////////////////////////////////////////////

#include "Infrastructure/VersionLock.hpp"
#include "Infrastructure/Protect.hpp"
#include "Infrastructure/Assert.hpp"
#include <fmt\format.h>
#include <fmt\core.h>

namespace Tixel::Adt
{
    template <typename TKey, typename TValue>
    class IndexNode final : public Infra::VersionLock
    {
		public:
    		using Guard = std::shared_ptr<IndexNode>;

	    public:
            explicit IndexNode():
				_key {},
				_value {},
				_next()
    		{}

            explicit IndexNode(const TKey& key, const TValue& value, Guard next):
                _key(key),
                _value(value),
				_next(next)
    		{}

            virtual ~IndexNode()
            {
                PROTECT_DESTRUCTOR(
                {
					TIXEL_ASSERT(!IsLocked());
                })
            }

			std::string ToString()
			{
				return fmt::format("[{}] [{}:{}] --> [{}]", fmt::ptr(this), _key, _value, fmt::ptr(_next.get()));
			}

		public:
	        TKey _key;
	        TValue _value;
            Guard _next;
    };
}