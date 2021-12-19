#include "Infrastructure/TixellInfra.hpp"
#include <boost/program_options.hpp>
#include <iostream>

enum ReturnValues : int32_t
{
	RvSuccess = 0,
	RvInvalidCmdLine = -1,
	RvGeneralFault = -2,
	RvCriticalFault = -3
};

int wmain(int, wchar_t const * const[])
{
    try
    {
        TRACE_INF("GVC: [{}]", Tixel::Infra::GlobalVersionClock::Read());
        TRACE_INF("GVC: [{}]", Tixel::Infra::GlobalVersionClock::AddAndFetch());
        TRACE_INF("GVC: [{}]", Tixel::Infra::GlobalVersionClock::AddAndFetch());
        TRACE_INF("GVC: [{}]", Tixel::Infra::GlobalVersionClock::Read());
        TRACE_INF("GVC: [{}]", Tixel::Infra::GlobalVersionClock::AddAndFetch());
        TRACE_INF("GVC: [{}]", Tixel::Infra::GlobalVersionClock::Read());

        return RvSuccess;
    }
    catch (const std::invalid_argument&)
    {
        return RvInvalidCmdLine;
    }
    catch (const std::exception& e)
    {
        std::cout << "General exception: [" << e.what() << "]" << std::endl;
        return RvGeneralFault;
    }
    catch (...)
    {
        std::cout << "Critical exception. Aborting" << std::endl;
        return RvCriticalFault;
    }
}