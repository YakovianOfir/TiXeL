#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | TxnListOperator.hpp |
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

#include "TxnListStorage.hpp"
#include "DataTypes/IList.hpp"

namespace Tixel::Txn
{
	template <typename TKey, typename TValue>
	class TxnListStorage;

	template <typename TKey, typename TValue>
	class TxnListOperator final : public Adt::IList<TKey, TValue>
	{
		private:
			friend class TxnList<TKey, TValue>;
			using List = TxnList<TKey, TValue>;
			using ListGuard = typename List::Guard;

		public:
			explicit TxnListOperator(Adt::Index<TKey, TValue>& index);

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
			NodeTuple TraverseTo(const TKey& key, ListStorage& localStorage) const;
			NodeGuard GetValidatedPredecessor(const TKey& key, ListStorage& localStorage) const;
			NodeGuard GetValidatedSuccessor(const NodeGuard& node, ListStorage& localStorage) const;

		private:
			ListGuard _list;
			Adt::Index<TKey, TValue>& _index;
	};

	template <typename TKey, typename TValue>
	TxnListOperator<TKey, TValue>::TxnListOperator(Adt::Index<TKey, TValue>& index): _index(index)
	{}

	template <typename TKey, typename TValue>
	void TxnListOperator<TKey, TValue>::SetList(const ListGuard& list)
	{
		_list = list;
	}

	template <typename TKey, typename TValue>
	bool TxnListOperator<TKey, TValue>::Contains(const TKey& key) const
	{
		TRACE_INF("[Txn-Operator]: Contains. (Key: [{}])", key);

		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		const auto [predecessor, successor] = TraverseTo(key, localStorage);

		return successor != nullptr && successor->_key == key;
	}

	template <typename TKey, typename TValue>
	std::optional<TValue> TxnListOperator<TKey, TValue>::Get(const TKey& key) const
	{
		TRACE_INF("[Txn-Operator]: Get. (Key: [{}])", key);

		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		const auto [predecessor, successor] = TraverseTo(key, localStorage);

		if (successor == nullptr || successor->_key != key)
		{
			return std::optional<TValue>();
		}

		return localStorage.GetUpdatedValue(successor);
	}

	template <typename TKey, typename TValue>
	std::optional<TValue> TxnListOperator<TKey, TValue>::Remove(const TKey& key)
	{
		TRACE_INF("[Txn-Operator]: Remove. (Key: [{}])", key);

		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		const auto [predecessor, successor] = TraverseTo(key, localStorage);

		if (successor == nullptr || successor->_key != key)
		{
			TRACE_WRN("[Txn-Operator]: Remove. (Key: [{}]) (NONE)", key);

			return std::optional<TValue>();
		}

		localStorage._writeSet.insert_or_assign(predecessor, WriteSetElement
		{
			GetValidatedSuccessor(successor, localStorage),
			false,
			localStorage.GetUpdatedValue(predecessor)
		});

		auto value = localStorage.GetUpdatedValue(successor);

		localStorage._writeSet.insert_or_assign(successor, WriteSetElement
		{
			std::make_shared<Node>(),
			true,
			value
		});

		localStorage._readSet.emplace(successor);

		localStorage._indexRemove[_list].emplace(successor);

		return value;
	}

	template <typename TKey, typename TValue>
	void TxnListOperator<TKey, TValue>::Insert(const TKey& key, const TValue& value)
	{
		TRACE_INF("[Txn-Operator]: Insert. (Key: [{}]) (Value: [{}])", key, value);

		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		const auto [predecessor, successor] = TraverseTo(key, localStorage);

		if (successor != nullptr && successor->_key == key)
		{
			const auto writeSetEntry = localStorage._writeSet.find(successor);

			localStorage._writeSet.insert_or_assign(successor, WriteSetElement
			{
				localStorage.GetUpdatedNext(successor),
				localStorage.GetUpdatedIsDeleted(successor),
				value
			});

			localStorage._readSet.emplace(successor);
		}
		else
		{
			auto candidateNode = std::make_shared<Node>(key, value, successor);

			candidateNode->SetSingleton(Infra::VersionLock::FlagOption::Disable);

			localStorage._writeSet.insert_or_assign(predecessor, WriteSetElement
			{
				candidateNode,
				false,
				localStorage.GetUpdatedValue(predecessor)
			});

			localStorage._indexAdd[_list].emplace_back(candidateNode);
		}
	}

	template <typename TKey, typename TValue>
	typename TxnListOperator<TKey, TValue>::NodeTuple TxnListOperator<TKey, TValue>::TraverseTo(const TKey& key, ListStorage& localStorage) const
	{
		auto predecessor = GetValidatedPredecessor(key, localStorage);

		auto successor = GetValidatedSuccessor(predecessor, localStorage);

		while (successor != nullptr)
		{
			if (successor->_key >= key)
			{
				break;
			}

			predecessor = successor;

			successor = GetValidatedSuccessor(predecessor, localStorage);
		}

		localStorage._readSet.emplace(predecessor);

		return { predecessor, successor };
	}

	template <typename TKey, typename TValue>
	typename TxnListOperator<TKey, TValue>::NodeGuard TxnListOperator<TKey, TValue>::GetValidatedSuccessor(const NodeGuard& node, ListStorage& localStorage) const
	{
		const auto writeSetEntry = localStorage._writeSet.find(node);

		if (writeSetEntry != localStorage._writeSet.end())
		{
			return writeSetEntry->second.next;
		}

		Infra::ROLLBACK_TXN(!node->IsLocked(), "Node is not expected to be locked.");

		const auto& next = node->_next;

		const auto readVersion = localStorage._state->readVersion;

		Infra::ROLLBACK_TXN(node->ValidVersionAndReleased(readVersion), "Node is expected to have a valid version.");

		return next;
	}

	template <typename TKey, typename TValue>
	typename TxnListOperator<TKey, TValue>::NodeGuard TxnListOperator<TKey, TValue>::GetValidatedPredecessor(const TKey& key, ListStorage& localStorage) const
	{
		auto candidateKey = key;

		while (true)
		{
			const auto& [predecessorKey, predecessor] = _index.GetPrevious(candidateKey);

			const auto readVersion = localStorage._state->readVersion;

			Infra::ROLLBACK_TXN(predecessor->ValidVersionAndReleased(readVersion), "Predecessor is expected to have a valid version.");

			if (!localStorage.GetUpdatedIsDeleted(predecessor))
			{
				return predecessor;
			}

			candidateKey = predecessorKey.Key();
		}
	}
}
