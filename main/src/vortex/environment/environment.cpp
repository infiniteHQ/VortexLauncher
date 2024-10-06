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

VORTEX_API void VortexMaker::RefreshEnvironmentProjects()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";

    std::string json_file = path + "/projects.json";

    // Clear project list
    ctx.IO.sys_projects.clear();

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        for (auto project : json_data["projects"])
        {
            std::shared_ptr<EnvProject> newproject = std::make_shared<EnvProject>();

            newproject->name = project["name"].get<std::string>();
            newproject->version = project["version"].get<std::string>();
            newproject->description = project["description"].get<std::string>();
            newproject->compatibleWith = project["compatibleWith"].get<std::string>();
            newproject->path = project["path"].get<std::string>();
            newproject->logoPath = project["logoPath"].get<std::string>();
            newproject->lastOpened = project["lastOpened"].get<std::string>();

            ctx.IO.sys_projects.push_back(newproject);
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
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
