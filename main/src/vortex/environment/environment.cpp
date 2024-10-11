#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::InitEnvironment()
{
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
        std::string file = path + "sessions.json";

        nlohmann::json default_data = {
            {"KeepSessionDataTimeout", "30d"}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/contents/modules/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/contents/templates/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/contents/templates/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/sessions/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/VortexProjects/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {
        std::string homeDir = VortexMaker::getHomeDirectory();
        std::string path = homeDir + "/.vx/configs/";
        std::string file = path + "projects_pools.json";

        nlohmann::json default_data = {
            {"projects_pools", nlohmann::json::array({homeDir + "/VortexProjects/"})}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }

    {
        std::string homeDir = VortexMaker::getHomeDirectory();
        std::string path = homeDir + "/.vx/configs/";
        std::string file = path + "modules_pools.json";

        nlohmann::json default_data = {
            {"modules_pools", nlohmann::json::array({homeDir + "/.vx/modules/"})}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }

    {
        std::string homeDir = VortexMaker::getHomeDirectory();
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
        std::string file = path + "plugins_pools.json";

        nlohmann::json default_data = {
            {"plugins_pools", nlohmann::json::array({homeDir + "/.vx/plugins/"})}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }

    {
        std::string homeDir = VortexMaker::getHomeDirectory();
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
        std::string file = path + "templates_pools.json";

        nlohmann::json default_data = {
            {"templates_pools", nlohmann::json::array({homeDir + "/.vx/templates/"})}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }

    {
        std::string homeDir = VortexMaker::getHomeDirectory();
        std::string path = homeDir + "/.vx/configs/";
        std::string file = path + "vortex_versions_pools.json";

        nlohmann::json default_data = {
            {"vortex_versions_pools", nlohmann::json::array({"/opt/Vortex/"})}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/sessions/";
        std::string file = path + "active_sessions.json";

        nlohmann::json default_data = {
            {"sessions", nlohmann::json::array()}};

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }

    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/contents/templates/vortex.templates.builtin.__blankproject/";
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

namespace fs = std::filesystem;

std::chrono::system_clock::time_point addTimeoutToTime(const std::string &time_str, const std::string &timeout)
{
    std::tm tm = {};
    std::stringstream ss(time_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    auto time = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    int number = std::stoi(timeout.substr(0, timeout.size() - 1));
    char unit = timeout.back();

    if (unit == 'h')
        return time + std::chrono::hours(number);
    else if (unit == 'd')
        return time + std::chrono::hours(number * 24);
    else if (unit == 'm')
        return time + std::chrono::hours(number * 24 * 30);
    else if (unit == 'y')
        return time + std::chrono::hours(number * 24 * 365);

    return time;
}

VORTEX_API void VortexMaker::UpdateSessions()
{
    std::string sessions_dir = "/home/diego/.vx/sessions";
    std::string config_path = "/home/diego/.vx/configs/sessions.json";

    std::ifstream config_file(config_path);
    nlohmann::json config;
    if (!config_file.is_open())
    {
        std::cerr << "Error." << std::endl;
        return;
    }
    config_file >> config;
    config_file.close();

    std::string timeout = config["KeepSessionDataTimeout"];

    for (const auto &entry : fs::directory_iterator(sessions_dir))
    {
        if (fs::is_directory(entry))
        {
            std::string session_path = entry.path().string();
            std::string session_json_path = session_path + "/session.json";

            std::ifstream session_file(session_json_path);
            if (session_file.is_open())
            {
                nlohmann::json session_data;
                session_file >> session_data;
                session_file.close();

                if (session_data.contains("SessionEndedAt"))
                {
                    std::string session_ended_at = session_data["SessionEndedAt"];

                    auto expiration_time = addTimeoutToTime(session_ended_at, timeout);
                    auto now = std::chrono::system_clock::now();

                    if (now > expiration_time)
                    {
                        fs::remove_all(session_path);
                    }
                }
            }
            else
            {
                std::cerr << "Error : " << session_json_path << std::endl;
            }
        }
    }
}

VORTEX_API void VortexMaker::RefreshEnvironmentProjectsPools()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

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

VORTEX_API void VortexMaker::RefreshEnvironmentModulesPools()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

    std::string json_file = path + "/modules_pools.json";

    // Clear project list
    ctx.IO.sys_modules_pools.clear();

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        for (auto pool : json_data["modules_pools"])
        {
            ctx.IO.sys_modules_pools.push_back(pool);
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
    }
}

VORTEX_API void VortexMaker::RefreshEnvironmentTemplatesPools()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

    std::string json_file = path + "/templates_pools.json";

    // Clear project list
    ctx.IO.sys_templates_pools.clear();

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        for (auto pool : json_data["templates_pools"])
        {
            ctx.IO.sys_templates_pools.push_back(pool);
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
    }
}

VORTEX_API void VortexMaker::RefreshEnvironmentPluginsPools()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

    std::string json_file = path + "/plugins_pools.json";

    // Clear project list
    ctx.IO.sys_plugins_pools.clear();

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        for (auto pool : json_data["plugins_pools"])
        {
            ctx.IO.sys_plugins_pools.push_back(pool);
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        VortexMaker::LogError("Error: ", e.what());
    }
}

VORTEX_API void VortexMaker::RefreshEnvironmentVortexVersionsPools()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

    std::string json_file = path + "/vortex_versions_pools.json";

    // Clear project list
    ctx.IO.sys_vortex_versions_pools.clear();

    // Verify if the project is valid
    try
    {
        // Load JSON data from the project configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        for (auto pool : json_data["vortex_versions_pools"])
        {
            ctx.IO.sys_vortex_versions_pools.push_back(pool);
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
    
    ctx.IO.sys_projects.clear();

    for (const auto &pool_path : ctx.IO.sys_projects_pools)
    {
        if (!std::filesystem::is_directory(pool_path))
        {
            continue;
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
                            if (std::filesystem::exists(logo_path))
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
