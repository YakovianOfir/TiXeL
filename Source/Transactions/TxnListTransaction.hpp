#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | TxnListTransaction.hpp |
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

namespace Tixel::Txn
{
	template <typename TKey, typename TValue>
	class TxnListStorage;

	template <typename TKey, typename TValue>
	class TxnListTransaction final : public Infra::NonCopyable
	{
		public:
			explicit TxnListTransaction() = delete;

		private:
			friend class TxnListStorage<TKey, TValue>;
			using ListStorage = TxnListStorage<TKey, TValue>;

		public:
			static void Begin();
			static void Commit();
			static void Rollback();
	};

	template <typename TKey, typename TValue>
	void TxnListTransaction<TKey, TValue>::Begin()
	{
		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		TRACE_VRB("[Txn]: Begin. (Local-Storage: [{}])", fmt::ptr(std::addressof(localStorage)));

		return localStorage.LocalTxnPrologue();
	}

	template <typename TKey, typename TValue>
	void TxnListTransaction<TKey, TValue>::Commit()
	{
		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		TRACE_VRB("[Txn]: Commit. (Local-Storage: [{}])", fmt::ptr(std::addressof(localStorage)));

		return localStorage.LocalTxnEpilogue();
	}

	template <typename TKey, typename TValue>
	void TxnListTransaction<TKey, TValue>::Rollback()
	{
		auto& localStorage = TxnListStorage<TKey, TValue>::GetLocalStorage();

		TRACE_VRB("[Txn]: Rollback. (Local-Storage: [{}])", fmt::ptr(std::addressof(localStorage)));

		return localStorage.LocalTxnCleanup();
	}
}
