#include "Trace.hpp"

namespace Tixel::Infra
{
    bool TroubleshootMode = false;

    void EnableTroubleshootMode()
    {
        TroubleshootMode = true;
    }

    std::string TraceLevelToString(const TraceLevel& level)
    {
        switch (level)
        {
            case TraceLevel::Information:
            {
                return "INF";
            }
            case TraceLevel::Warning:
            {
                return "WRN";
            }
            case TraceLevel::Error:
            {
                return "ERR";
            }
            case TraceLevel::Verbose:
            {
                return "VRB";
            }
        }

        throw std::invalid_argument(fmt::format("Unsupported trace level: {}", level));
    }
}