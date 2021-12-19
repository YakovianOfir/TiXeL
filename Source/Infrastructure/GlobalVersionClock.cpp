#include "GlobalVersionClock.hpp"

namespace Tixel::Infra
{

GlobalVersionClock::GlobalVersionClock(): _gvc(0)
{}

GlobalVersionClock& GlobalVersionClock::Instance()
{
    static GlobalVersionClock gvcInstance;

	return gvcInstance;
}

uint64_t GlobalVersionClock::Read()
{
    return Instance()._gvc.load();
}

uint64_t GlobalVersionClock::AddAndFetch()
{
    return std::atomic_fetch_add<uint64_t>(&Instance()._gvc, 1) + 1;
}

}