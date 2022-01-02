#include "LockBase.hpp"
#include "Exception.hpp"

namespace Tixel::Infra
{

void LockBase::Acquire()
{
	ROLLBACK_TXN(TryAcquire(), "Cannot acquire lock object.");
}

}