#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::InitEnvironment()
{
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        VortexMaker::createFolderIfNotExists(path);
    }
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/modules/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/templates/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/templates/vortex.templates.builtin.__blankproject/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        std::string file = path + "projects.json";

        nlohmann::json default_data = {
            {"projects", nlohmann::json::array()}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }
}

VORTEX_API void VortexMaker::RefreshEnvironmentProjectsPools()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";

    std::string json_file = path + "/projects_pools.json";

    // Clear project list
    ctx.IO.sys_projects_pools.clear();

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        for (auto pool : json_data["projects_pools"])
        {
            ctx.IO.sys_projects_pools.push_back(pool);
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
    }
}

void VortexMaker::RefreshEnvironmentProjects()
{
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
    std::string json_file = path + "/projects.json";

    ctx.IO.sys_projects.clear();

    for (const auto &pool_path : ctx.IO.sys_projects_pools)
    {
        // Check if pool_path is a directory before proceeding
        if (!std::filesystem::is_directory(pool_path))
        {
            continue; // Skip to the next path
        }

        for (const auto &entry : std::filesystem::recursive_directory_iterator(pool_path))
        {
            if (entry.is_regular_file() && entry.path().filename() == "vortex.config")
            {
                try
                {
                    std::ifstream file(entry.path());
                    if (file.is_open())
                    {
                        nlohmann::json json_data;
                        file >> json_data;

                        if (json_data.contains("project"))
                        {
                            nlohmann::json project_data = json_data["project"];
                            std::shared_ptr<EnvProject> project = std::make_shared<EnvProject>();

                            project->name = project_data.value("name", "");
                            project->path = entry.path().parent_path().string();
                            project->version = project_data.value("version", "");
                            project->compatibleWith = project_data.value("compatibleWith", "");
                            project->description = project_data.value("description", "");             

                            std::string logo_path = entry.path().parent_path().string() + "/" + project_data.value("logoPath", "");
                            if(std::filesystem::exists(logo_path))
                            {
                                project->logoPath = entry.path().parent_path().string() + "/" + project_data.value("logoPath", "");
                            }           
                            
                            project->author = project_data.value("author", "");
                            project->lastOpened = project_data.value("lastOpened", "unknown");

                            ctx.IO.sys_projects.push_back(project);
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    // Print error if an exception occurs
                    VortexMaker::LogError("Error: ", e.what());
                }
            }
        }
    }
}

VORTEX_API void VortexMaker::UpdateEnvironmentProject(const std::string &name, const std::string &author, const std::string &version, const std::string &compatibleWith, const std::string &description, const std::string &path, const std::string &logo_path, const std::string &template_name)
{ // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string sys_path = VortexMaker::getHomeDirectory() + "/.vx/data/";
    std::string json_file = sys_path + "/projects.json";

    // Verify if the project is valid
    // Load JSON data from the project configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);

    VortexMaker::LogInfo("Core", "Verify if the new project \"" + name + "\" is not already existing.");

    // Check if a project with the given name exists
    bool projectExists = false;
    for (auto &project : json_data["projects"])
    {
        if (project["name"].get<std::string>() == name)
        {
            projectExists = true;
            return;
        }
    }

    VortexMaker::LogInfo("Core", "Create a new entry in registered projects for \"" + name + "\".");

    // If the project doesn't exist, create a new JSON object and add it to the list
    json_data["projects"].push_back({{"name", name},
                                     {"version", version},
                                     {"description", description},
                                     {"path", path},
                                     {"lastOpened", VortexMaker::getCurrentTimeStamp()},
                                     {"compatibleWith", compatibleWith},
                                     {"logoPath", logo_path}});

    // Write the updated JSON data back to the file
    std::ofstream output(json_file);
    output << json_data.dump(4); // Use pretty print with indentation of 4 spaces
    output.close();
}

VORTEX_API void VortexMaker::UpdateEnvironmentProject()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
    std::string json_file = path + "/projects.json";

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);

        std::string name = ctx.name;

        // Check if a project with the given name exists
        bool projectExists = false;
        for (auto &project : json_data["projects"])
        {
            if (project["name"].get<std::string>() == name)
            {
                // Project with the given name exists, update its information
                project["version"] = ctx.project_version;
                project["description"] = ctx.description;
                project["path"] = ctx.projectPath;
                project["compatibleWith"] = ctx.compatibleWith;
                project["lastOpened"] = VortexMaker::getCurrentTimeStamp();
                project["logoPath"] = ctx.logoPath;
                projectExists = true;
                break;
            }
        }

        // If the project doesn't exist, create a new JSON object and add it to the list
        if (!projectExists)
        {
            json_data["projects"].push_back({{"name", ctx.name},
                                             {"version", ctx.project_version},
                                             {"description", ctx.description},
                                             {"path", ctx.projectPath},
                                             {"lastOpened", VortexMaker::getCurrentTimeStamp()},
                                             {"compatibleWith", ctx.compatibleWith},
                                             {"logoPath", ctx.logoPath}});
        }

        // Write the updated JSON data back to the file
        std::ofstream output(json_file);
        output << json_data.dump(4); // Use pretty print with indentation of 4 spaces
        output.close();
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
    }
}

VORTEX_API void VortexMaker::UpdateEnvironmentProject(const std::string &oldname)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
    std::string json_file = path + "/projects.json";

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);

        // Check if a project with the old name exists
        bool projectExists = false;
        for (auto &project : json_data["projects"])
        {
            if (project["name"].get<std::string>() == oldname)
            {
                // Project with the old name exists, update its information
                project["name"] = ctx.name;
                project["version"] = ctx.version;
                project["description"] = ctx.description;
                project["path"] = ctx.projectPath;
                project["logoPath"] = ctx.logoPath;
                projectExists = true;
                break;
            }
        }

        // If the project doesn't exist, create a new JSON object and add it to the list
        if (!projectExists)
        {
            json_data["projects"].push_back({{"name", ctx.name},
                                             {"version", ctx.version},
                                             {"description", ctx.description},
                                             {"path", ctx.projectPath},
                                             {"logoPath", ctx.logoPath}});
        }

        // Write the updated JSON data back to the file
        std::ofstream output(json_file);
        output << json_data.dump(4); // Use pretty print with indentation of 4 spaces
        output.close();
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
    }
}
