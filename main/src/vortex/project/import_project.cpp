#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief
 */
VORTEX_API std::vector<std::shared_ptr<EnvProject>> VortexMaker::FindProjectInFolder(const std::string &path)
{
    // Search for module files recursively in the directory
    auto module_files = VortexMaker::SearchFiles(path, "vortex.config", 3);

    // Init the array
    std::vector<std::shared_ptr<EnvProject>> projects;

    // Iterate through each found module file
    for (const auto &file : module_files)
    {
        // Load JSON data from the module configuration file
        auto json_data = VortexMaker::DumpJSON(file);

        if(json_data.empty())
        continue;

        std::shared_ptr<EnvProject> newproject = std::make_shared<EnvProject>();

        newproject->name = json_data["name"].get<std::string>();
        newproject->version = json_data["version"].get<std::string>();
        newproject->description = json_data["description"].get<std::string>();
        newproject->compatibleWith = json_data["compatibleWith"].get<std::string>();
        newproject->path = "path - file";
        newproject->logoPath = json_data["logoPath"].get<std::string>();
        newproject->lastOpened = json_data["lastOpened"].get<std::string>();

        projects.push_back(newproject);
    }
    return projects;
}

/**
 * @brief
 */
VORTEX_API void VortexMaker::ImportProject(const std::string &path)
{
    // Search for module files recursively in the directory
    auto module_files = VortexMaker::SearchSystemFiles(path, "vortex.config");

    // Iterate through each found module file
    for (const auto &file : module_files)
    {
    }
}