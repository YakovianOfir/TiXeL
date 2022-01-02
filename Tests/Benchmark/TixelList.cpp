#include "TixelList.hpp"
#include "Transactions/TixelTxn.hpp"
#include <memory>

intset_l_t *set_new_l()
{
	const auto txnListPointer =
		new std::shared_ptr<Tixel::Txn::TxnList<int, int>>(
			std::make_shared<Tixel::Txn::TxnList<int, int>>());

	txnListPointer->get()->ImbueOperators();

	return txnListPointer;
}

void set_delete_l(intset_l_t *set)
{
	auto * txnListPointer = static_cast<std::shared_ptr<Tixel::Txn::TxnList<int, int>>*>(set);

    delete txnListPointer;
}

int set_size_l(intset_l_t *set)
{
	auto * txnListPointer = static_cast<std::shared_ptr<Tixel::Txn::TxnList<int, int>>*>(set);

	return static_cast<int>(txnListPointer->get()->Size());
}