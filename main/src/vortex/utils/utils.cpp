#include "../../../include/vortex.h"

VORTEX_API std::string VortexMaker::getCurrentTimeStamp()
{
    // Get actual time
    std::time_t currentTime = std::time(nullptr);

    // Convert to tm
    std::tm *localTime = std::localtime(&currentTime);

    // Format timestamp
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}