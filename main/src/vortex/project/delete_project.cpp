#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief CreateProject creates a new project with the specified name and path.
 *
 * This function creates a new project directory structure and initializes necessary files
 * such as 'vortex.config'.
 *
 * @param name The name of the project.
 * @param path The path where the project will be created.
 */
VORTEX_API void VortexMaker::DeleteProject(const std::string &path, const std::string &project_name)
{
    // Vérifiez si le fichier existe avant de l'ouvrir
    if (!std::filesystem::exists(path + "/vortex.config"))
    {
        // Lancez une exception si le fichier n'existe pas
        VortexMaker::LogError("Core", "Project not found (File not found: " + path + "/vortex.config)");
        VortexMaker::LogWarn("Core", "Delete the project entry of environment.");
        VortexMaker::RemoveSystemProjectEntry(project_name);
        return;
    }

    // Load JSON data from the module configuration file
    auto json_data = VortexMaker::DumpJSON(path + "/vortex.config");

    if (!json_data.empty() || json_data == "{}")
    {
        if (!json_data["project"]["name"].get<std::string>().empty())
        {
            if (json_data["project"]["name"].get<std::string>() == project_name)
            {
                std::string project_path = path;

                {
                    std::string cmd = "rm -rf " + project_path;
                    if (std::system(cmd.c_str()) == 0)
                    {
                        VortexMaker::LogInfo("Core", "The project \"" + json_data["project"]["name"].get<std::string>() + "\" is succefully deleted !");
                        VortexMaker::RemoveSystemProjectEntry(project_name);
                        return;
                    }
                    else
                    {
                        VortexMaker::LogError("Core", "Failed to delete the project \"" + json_data["project"]["name"].get<std::string>() + "\" at path \"" + json_data["path"].get<std::string>() + "\"");                        return;
                    }
                }
            }
        }
    }

    VortexMaker::LogError("Core", "No project to delete at \"" + path + "\"");
}

VORTEX_API void VortexMaker::RemoveSystemProjectEntry(const std::string& project_name)
{
    std::string dataPath = VortexMaker::getHomeDirectory() + "/.vx/data/";
    std::string configFile = dataPath + "/projects.json";

    // Load JSON data from the module configuration file
    auto json_data = VortexMaker::DumpJSON(configFile);

    if (!json_data.empty())
    {
        auto& projects = json_data["projects"];
        for (auto it = projects.begin(); it != projects.end(); ++it)
        {
            if ((*it)["name"].get<std::string>() == project_name)
            {
                projects.erase(it);
                break; // Assuming project names are unique, we can break after finding the project
            }
        }
    }
    else
    {
        return;
    }

    // Write the updated JSON data back to the file
    std::ofstream output(configFile);
    output << json_data.dump(4); // Use pretty print with indentation of 4 spaces
    output.close();
}
