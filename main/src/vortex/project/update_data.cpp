#include "../../../include/vortex.h"

VORTEX_API void VortexMaker::UpdateProjectData(const std::string& old_name, const std::string& path)
{
    std::string dataPath = VortexMaker::getHomeDirectory() + "/.vx/data/";
    VortexMaker::createFolderIfNotExists(dataPath);

    std::string configFile = dataPath + "/projects.json";
}