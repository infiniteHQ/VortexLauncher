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

std::vector<int> VortexMaker::SplitVersion(const std::string &version)
{
    std::vector<int> versionParts;
    std::stringstream ss(version);
    std::string part;

    while (std::getline(ss, part, '.'))
    {
        versionParts.push_back(std::stoi(part));
    }

    while (versionParts.size() < 3)
    {
        versionParts.push_back(0);
    }

    return versionParts;
}

bool VortexMaker::IsVersionGreater(const std::string &version, const std::string &comparate_version)
{
    std::vector<int> v1 = VortexMaker::SplitVersion(version);
    std::vector<int> v2 = VortexMaker::SplitVersion(comparate_version);

    for (size_t i = 0; i < 3; ++i)
    {
        if (v2[i] > v1[i])
        {
            return true;
        }
        else if (v2[i] < v1[i])
        {
            return false;
        }
    }

    return false;
}

std::string VortexMaker::NormalizeVersion(const std::string &version)
{
    std::vector<int> versionParts = VortexMaker::SplitVersion(version);

    std::stringstream ss;
    ss << versionParts[0] << "." << versionParts[1] << "." << versionParts[2];

    return ss.str();
}