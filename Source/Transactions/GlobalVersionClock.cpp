#include "GlobalVersionClock.hpp"

namespace Tixel::Txn
{

GlobalVersionClock::GlobalVersionClock(): _gvc(0)
{}

GlobalVersionClock& GlobalVersionClock::Instance()
{
    static GlobalVersionClock gvcInstance;

	return gvcInstance;
}

GlobalVersionClock::Version GlobalVersionClock::Read()
{
    return Instance()._gvc.load();
}

GlobalVersionClock::Version GlobalVersionClock::AddAndFetch()
{
    return std::atomic_fetch_add<Version>(&Instance()._gvc, 1) + 1;
}

}