#include "VersionLock.hpp"
#include "Assert.hpp"
#include "Flags.hpp"

namespace Tixel::Infra
{

VersionLock::VersionLock(): _versionAndFlags(0)
{}

bool VersionLock::TryAcquire()
{
	auto lockValue = Load();

	if (IsFlagSet(lockValue, LockStateMask))
	{
		return false;
	}

	const auto acquiredValue = lockValue | LockStateMask;

	return _versionAndFlags.compare_exchange_strong(lockValue, acquiredValue);
}

void VersionLock::Release()
{
	auto lockValue = Load();

	TIXEL_ASSERT(IsFlagSet(lockValue, LockStateMask));

	const auto releasedValue = lockValue & (~LockStateMask);

	TIXEL_ASSERT(_versionAndFlags.compare_exchange_strong(lockValue, releasedValue));
}

void VersionLock::SetVersion(const uint64_t version)
{
	auto lockValue = Load();

	lockValue &= VersionNegativeMask;
	lockValue |= (version & (~VersionNegativeMask));

	Store(lockValue);
}

void VersionLock::SetDeleted(const FlagOption option)
{
	auto lockValue = Load();

	if (option == FlagOption::Enable)
	{
		SetFlag(lockValue, DeleteStateMask);
	}
	else if (option == FlagOption::Disable)
	{
		ClearFlag(lockValue, DeleteStateMask);
	}

	Store(lockValue);
}

void VersionLock::SetSingleton(const FlagOption option)
{
	auto lockValue = Load();

	if (option == FlagOption::Enable)
	{
		SetFlag(lockValue, SingletonStateMask);
	}
	else if (option == FlagOption::Disable)
	{
		ClearFlag(lockValue, SingletonStateMask);
	}

	Store(lockValue);
}

uint64_t VersionLock::GetVersion() const
{
	return Load() & (~VersionNegativeMask);
}

bool VersionLock::IsLocked() const
{
	return IsFlagSet(Load(), LockStateMask);
}

bool VersionLock::IsDeleted() const
{
	return IsFlagSet(Load(), DeleteStateMask);
}

bool VersionLock::IsSingleton() const
{
	return IsFlagSet(Load(), SingletonStateMask);
}

bool VersionLock::IsLockedOrDeleted() const
{
	const auto lockValue = Load();

	return IsFlagSet(lockValue, LockStateMask) || IsFlagSet(lockValue, DeleteStateMask);
}

bool VersionLock::ValidVersionAndReleased(uint64_t version) const
{
	const auto lockValue = Load();
	const auto lockVersion = lockValue & (~VersionNegativeMask);

	return !IsFlagSet(lockValue, LockStateMask) && (lockVersion <= version);
}

uint64_t VersionLock::Load() const
{
	return _versionAndFlags.load();
}

void VersionLock::Store(const uint64_t value)
{
	_versionAndFlags.store(value);
}

uint64_t VersionLock::LockStateMask = 0x1000000000000000L;
uint64_t VersionLock::DeleteStateMask = 0x2000000000000000L;
uint64_t VersionLock::SingletonStateMask = 0x4000000000000000L;
uint64_t VersionLock::VersionNegativeMask = LockStateMask | DeleteStateMask | SingletonStateMask;

}
