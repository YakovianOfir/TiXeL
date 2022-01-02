#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | TxnListStorage.hpp |
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

#include <vector>
#include <unordered_set>
#include "GlobalVersionClock.hpp"
#include "Infrastructure/Exception.hpp"
#include "Infrastructure/Locker.hpp"
#include "Infrastructure/Assert.hpp"
#include "Infrastructure/Trace.hpp"
#include "DataTypes/Index.hpp"

namespace Tixel::Txn
{
	template <typename TKey, typename TValue>
	class TxnListOperator;

	template <typename TKey, typename TValue>
	class SingleListOperator;

	template <typename TKey, typename TValue>
	class TxnList;

	template <typename TKey, typename TValue>
	class TxnListStorage final : public Infra::NonCopyable
	{
		public:
			explicit TxnListStorage();

		private:
			friend class TxnList<TKey, TValue>;
			using List = TxnList<TKey, TValue>;
			using ListGuard = typename List::Guard;

		private:
			friend class Adt::IndexNode<TKey, TValue>;
			using Node = Adt::IndexNode<TKey, TValue>;
			using NodeGuard = typename Node::Guard;

		private:
			friend class TxnListOperator<TKey, TValue>;
			friend class SingleListOperator<TKey, TValue>;

		private:
			struct WriteSetElement
			{
				NodeGuard next {nullptr};
				bool deleted {false};
				TValue value {};
			};

			struct TxnLocalState
			{
				bool txnActive {false};
				GlobalVersionClock::Version writeVersion {0L};
				GlobalVersionClock::Version readVersion {0L};
			};

		public:
			static TxnListStorage& GetLocalStorage();

		public:
			bool LocalTxnIsActive();
			void LocalTxnPrologue();
			void LocalTxnEpilogue();
			void LocalTxnCleanup();

		public:
			TValue GetUpdatedValue(const NodeGuard& node);
			NodeGuard GetUpdatedNext(const NodeGuard& node);
			bool GetUpdatedIsDeleted(const NodeGuard& node);

		private:
			void UpdateWriteSet();
			bool ValidateReadSet();
			void LazyUpdateIndexAdd();
			void LazyUpdateIndexRemove();
			void IncrementWriteVersion();
			std::vector<std::unique_ptr<Infra::Locker>> LockWriteSet();

		private:
			std::unique_ptr<TxnLocalState> _state;
			std::unordered_set<NodeGuard> _readSet;
			std::unordered_map<NodeGuard, WriteSetElement> _writeSet;
			std::unordered_map<ListGuard, std::vector<NodeGuard>> _indexAdd;
			std::unordered_map<ListGuard, std::unordered_set<NodeGuard>> _indexRemove;
	};

	template <typename TKey, typename TValue>
	TxnListStorage<TKey, TValue>::TxnListStorage():
		_state(std::make_unique<TxnLocalState>())
	{}

	template <typename TKey, typename TValue>
	TxnListStorage<TKey, TValue>& TxnListStorage<TKey, TValue>::GetLocalStorage()
	{
		thread_local static TxnListStorage<TKey, TValue> txnLocalStorage;

		TRACE_VRB("[Local-Txn]: Get. (Storage: [{}])", fmt::ptr(std::addressof(txnLocalStorage)));

		return txnLocalStorage;
	}

	template <typename TKey, typename TValue>
	bool TxnListStorage<TKey, TValue>::LocalTxnIsActive()
	{
		TRACE_VRB("[Local-Txn]: Is Active. (Storage: [{}]) (State: [{}])", fmt::ptr(this), _state->txnActive ? 'Y' : 'N');

		return _state->txnActive;
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::LocalTxnPrologue()
	{
		TRACE_VRB("[Local-Txn]: Prologue. (Storage: [{}])", fmt::ptr(this));

		TIXEL_ASSERT(!_state->txnActive);

		_state->readVersion = GlobalVersionClock::Read();

		TRACE_VRB("[Local-Txn]: Prologue. (Read-Version: [{}]) (Storage: [{}])", _state->readVersion, fmt::ptr(this));

		_state->txnActive = true;
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::LocalTxnEpilogue()
	{
		TRACE_VRB("[Local-Txn]: Epilogue. (Storage: [{}])", fmt::ptr(this));

		TIXEL_ASSERT(_state->txnActive);

		TRACE_VRB("[Local-Txn]: Epilogue. (Lock Write Set) (Storage: [{}])", fmt::ptr(this));

		auto locks = LockWriteSet();

		TRACE_VRB("[Local-Txn]: Epilogue. (Validate Read Set) (Storage: [{}])", fmt::ptr(this));

		Infra::ROLLBACK_TXN(ValidateReadSet(), "Invalid Read Set.");

		TRACE_VRB("[Local-Txn]: Epilogue. (Increment Write Version) (Storage: [{}])", fmt::ptr(this));

		IncrementWriteVersion();

		TRACE_VRB("[Local-Txn]: Epilogue. (Update Write Set) (Write-Version: [{}]) (Storage: [{}])", _state->writeVersion, fmt::ptr(this));

		UpdateWriteSet();

		TRACE_VRB("[Local-Txn]: Epilogue. (Unlock Write Set) (Storage: [{}])", fmt::ptr(this));

		locks.clear();

		TRACE_VRB("[Local-Txn]: Epilogue. (Lazy Update Index [Add]) (Storage: [{}])", fmt::ptr(this));

		LazyUpdateIndexAdd();

		TRACE_VRB("[Local-Txn]: Epilogue. (Lazy Update Index [Remove]) (Storage: [{}])", fmt::ptr(this));

		LazyUpdateIndexRemove();

		TRACE_VRB("[Local-Txn]: Epilogue. (Cleanup) (Storage: [{}])", fmt::ptr(this));

		LocalTxnCleanup();
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::LocalTxnCleanup()
	{
		TRACE_VRB("[Local-Txn]: Cleanup. (Storage: [{}])", fmt::ptr(this));

		_state = std::make_unique<TxnLocalState>();

		TRACE_VRB("[Local-Txn]: Cleanup. (Clear State) (Storage: [{}])", fmt::ptr(this));

		_readSet.clear();

		TRACE_VRB("[Local-Txn]: Cleanup. (Clear Read Set) (Storage: [{}])", fmt::ptr(this));

		_writeSet.clear();

		TRACE_VRB("[Local-Txn]: Cleanup. (Clear Write Set) (Storage: [{}])", fmt::ptr(this));

		_indexAdd.clear();

		TRACE_VRB("[Local-Txn]: Cleanup. (Clear Index [Add]) (Storage: [{}])", fmt::ptr(this));

		_indexRemove.clear();

		TRACE_VRB("[Local-Txn]: Cleanup. (Clear Index [Remove]) (Storage: [{}])", fmt::ptr(this));
	}

	template <typename TKey, typename TValue>
	std::vector<std::unique_ptr<Infra::Locker>> TxnListStorage<TKey, TValue>::LockWriteSet()
	{
		std::vector<std::unique_ptr<Infra::Locker>> lockedNodes;

		for (auto& [node, writeSetElement]: _writeSet)
		{
			lockedNodes.push_back(std::make_unique<Infra::Locker>(*node));
		}

		return lockedNodes;
	}

	template <typename TKey, typename TValue>
	bool TxnListStorage<TKey, TValue>::ValidateReadSet()
	{
		for (const auto& node : _readSet)
		{
			const bool inWriteSet = _writeSet.find(node) != _writeSet.end();

			if (!inWriteSet && node->IsLocked())
			{
				return false;
			}

			if (node->GetVersion() > _state->readVersion)
			{
				return false;
			}

			if (node->GetVersion() == _state->readVersion && node->IsSingleton())
			{
				GlobalVersionClock::AddAndFetch();

				node->SetSingleton(Infra::VersionLock::FlagOption::Disable);

				return false;
			}

			if (!inWriteSet && node->IsLocked())
			{
				return false;
			}
		}

		_readSet.clear();

		return true;
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::IncrementWriteVersion()
	{
		auto writeVersion = GlobalVersionClock::AddAndFetch();

		TIXEL_ASSERT(writeVersion > _state->readVersion);

		_state->writeVersion = writeVersion;
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::UpdateWriteSet()
	{
		for (auto& [node, writeSetElement] : _writeSet)
		{
			node->_next = writeSetElement.next;
			node->_value = writeSetElement.value;

			if (writeSetElement.deleted)
			{
				node->SetDeleted(Infra::VersionLock::FlagOption::Enable);
			}

			node->SetVersion(_state->writeVersion);
			node->SetSingleton(Infra::VersionLock::FlagOption::Disable);
		}

		_writeSet.clear();
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::LazyUpdateIndexAdd()
	{
		for (auto& [list, nodes] : _indexAdd)
		{
			list->IndexAdd(nodes);
		}

		_indexAdd.clear();
	}

	template <typename TKey, typename TValue>
	void TxnListStorage<TKey, TValue>::LazyUpdateIndexRemove()
	{
		for (auto& [list, nodes] : _indexRemove)
		{
			list->IndexRemove(nodes);
		}

		_indexRemove.clear();
	}

	template <typename TKey, typename TValue>
	bool TxnListStorage<TKey, TValue>::GetUpdatedIsDeleted(const NodeGuard& node)
	{
		const auto writeSetEntry = _writeSet.find(node);

		return writeSetEntry == _writeSet.end() ? node->IsDeleted() : writeSetEntry->second.deleted;
	}

	template <typename TKey, typename TValue>
	TValue TxnListStorage<TKey, TValue>::GetUpdatedValue(const NodeGuard& node)
	{
		const auto writeSetEntry = _writeSet.find(node);

		return writeSetEntry == _writeSet.end() ? node->_value : writeSetEntry->second.value;
	}

	template <typename TKey, typename TValue>
	typename TxnListStorage<TKey, TValue>::NodeGuard TxnListStorage<TKey, TValue>::GetUpdatedNext(const NodeGuard& node)
	{
		const auto writeSetEntry = _writeSet.find(node);

		return writeSetEntry == _writeSet.end() ? node->_next : writeSetEntry->second.next;
	}
}
