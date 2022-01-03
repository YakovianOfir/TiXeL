/*
 * File:
 *   intset.c
 * Author(s):
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Linked list integer set operations
 *
 * Copyright (c) 2009-2010.
 *
 * intset.c is part of Synchrobench
 *
 * Synchrobench is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "IntSet.hpp"
#include "../../Source/Transactions/TixelTxn.hpp"
#include <memory>

int set_contains_l(intset_l_t *set, val_t val, bool transactional)
{
	auto * txnListPointer = static_cast<std::shared_ptr<Tixel::Txn::TxnList<int, int>>*>(set);

	auto contains = [&] () { (void) txnListPointer->get()->Contains(val); };

	if (transactional)
	{
		return Tixel::Txn::TransactionDispatcher<int, int>::TryDispatch(contains);
	}

    contains();

    return true;
}

int set_add_l(intset_l_t *set, val_t val, bool transactional)
{
	auto * txnListPointer = static_cast<std::shared_ptr<Tixel::Txn::TxnList<int, int>>*>(set);

	auto insert = [&] () { (void) txnListPointer->get()->Insert(val, 0); };

	if (transactional)
	{
		return Tixel::Txn::TransactionDispatcher<int, int>::TryDispatch(insert);
	}

    insert();

    return true;
}

int set_remove_l(intset_l_t *set, val_t val, bool transactional)
{
	auto * txnListPointer = static_cast<std::shared_ptr<Tixel::Txn::TxnList<int, int>>*>(set);

	auto remove = [&] () { (void) txnListPointer->get()->Remove(val); };

	if (transactional)
	{
		return Tixel::Txn::TransactionDispatcher<int, int>::TryDispatch(remove);
	}

    remove();

    return true;
}
