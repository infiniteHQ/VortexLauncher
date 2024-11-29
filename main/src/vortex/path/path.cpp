#include "../../../include/vortex.h"

void VortexMaker::createFolderIfNotExists(const std::string &path)
{
    if (!fs::exists(path))
    {
        try
        {
            std::string cmd = "mkdir " + path;
            system(cmd.c_str());
            VortexMaker::LogInfo("Core", "Path '" + path + "' created with success.");
        }
        catch (const std::exception &ex)
        {
            VortexMaker::LogError("Core", "Error while creating folder '" + path + "'");
            VortexMaker::LogError("Core", ex.what());
        }
    }
    else
    {
        VortexMaker::LogInfo("Core", "Path '" + path + "' already exist.");
    }
}

void VortexMaker::createJsonFileIfNotExists(const std::string &filename, const nlohmann::json &defaultData)
{
    try
    {
        fs::path filePath(filename);

        if (filePath.has_parent_path() && !fs::exists(filePath.parent_path()))
        {
            fs::create_directories(filePath.parent_path());
        }

        if (!fs::exists(filePath))
        {
            std::ofstream file(filePath);
            if (!file.is_open())
            {
                throw std::ios_base::failure("Unable to open file for writing.");
            }

            file << std::setw(4) << defaultData << std::endl;
            VortexMaker::LogInfo("Core", "JSON file '" + filename + "' created with default data.");
        }
        else
        {
            VortexMaker::LogInfo("Core", "JSON file '" + filename + "' already exists.");
        }
    }
    catch (const fs::filesystem_error &fserr)
    {
        VortexMaker::LogError("Core", "Filesystem error while handling JSON file '" + filename + "': " + fserr.what());
    }
    catch (const std::exception &ex)
    {
        VortexMaker::LogError("Core", "Error while creating JSON file '" + filename + "': " + ex.what());
    }
}