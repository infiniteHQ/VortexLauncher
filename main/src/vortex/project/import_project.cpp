#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief
 */
VORTEX_API std::vector<std::shared_ptr<EnvProject>> VortexMaker::FindProjectInFolder(const std::string &path)
{
    auto project_files = VortexMaker::SearchFiles(path, "vortex.config", 3);

    // Init the array
    std::vector<std::shared_ptr<EnvProject>> projects;

    // Iterate through each found module file
    for (const auto &file : project_files)
    {
        // Load JSON data from the module configuration file
        auto json_data = VortexMaker::DumpJSON(file);

        if (json_data.empty())
            continue;

        std::shared_ptr<EnvProject> newproject = std::make_shared<EnvProject>();

        newproject->name = json_data["project"]["name"].get<std::string>();
        newproject->version = json_data["project"]["version"].get<std::string>();
        newproject->description = json_data["project"]["description"].get<std::string>();
        newproject->compatibleWith = json_data["project"]["compatibleWith"].get<std::string>();
        std::string project_path = file.substr(0, file.find_last_of("/"));

        newproject->path = project_path;
        newproject->logoPath = json_data["project"]["logoPath"].get<std::string>();
        newproject->lastOpened = json_data["project"]["lastOpened"].get<std::string>();

        projects.push_back(newproject);
    }
    return projects;
}

/**
 * @brief
 */
VORTEX_API void VortexMaker::ImportProject(const std::string &path, const std::string &pool_path)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Get the home directory
    std::string homeDir = VortexMaker::getHomeDirectory();

    // Search modules registered
    auto module_files = VortexMaker::SearchFiles(path, "vortex.config");

    // Finded state
    bool finded = false;

    for (const auto &file : module_files)
    {
        try
        {
            std::string project_path = file.substr(0, file.find_last_of("/"));

            finded = true;

            // Move the module into the project
            {
                std::string cmd = "cp -r " + project_path + " " + pool_path;
                system(cmd.c_str());
            }

            return;
        }
        catch (std::exception e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }
}

VORTEX_API void VortexMaker::FindpProjectsInDirectoryRecursively(const std::string &directory, std::vector<std::shared_ptr<EnvProject>> &projects, std::atomic<bool> &stillSearching, std::string &elapsedTime)
{
    std::thread searchThread([directory, &projects, &stillSearching]()
                             {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
        {
            VortexMaker::LogError("Core", "Directory does not exist or is not a directory: " + directory);
            stillSearching = false;
            return;
        }

        try
        {
            for (const auto &entry : std::filesystem::recursive_directory_iterator(directory))
            {
                if (!stillSearching) return;

                if (entry.is_regular_file() && entry.path().filename() == "vortex.config")
                {
                    try
                    {
                        auto json_data = VortexMaker::DumpJSON(entry.path().string());

                        std::string module_path = entry.path().parent_path().string();
                        
        std::shared_ptr<EnvProject> newproject = std::make_shared<EnvProject>();

        newproject->name = json_data["project"]["name"].get<std::string>();
        newproject->version = json_data["project"]["version"].get<std::string>();
        newproject->description = json_data["project"]["description"].get<std::string>();
        newproject->compatibleWith = json_data["project"]["compatibleWith"].get<std::string>();
        std::string path = entry.path();
        std::string project_path = path.substr(0, path.find_last_of("/"));

        newproject->path = project_path;
        newproject->logoPath = json_data["project"]["logoPath"].get<std::string>();
        newproject->lastOpened = json_data["project"]["lastOpened"].get<std::string>();

        projects.push_back(newproject);
        
                    }
                    catch (const std::exception &e)
                    {
                        VortexMaker::LogError("Core", "Error processing file " + entry.path().string() + ": " + e.what());
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            VortexMaker::LogError("Core", "Filesystem error during recursive search: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            VortexMaker::LogError("Core", "General error during recursive search: " + std::string(e.what()));
        }

        stillSearching = false; });

    std::thread timerThread([&]()
                            {
        auto start = std::chrono::steady_clock::now();
        while (stillSearching)
        {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start);

            std::ostringstream oss;
            oss << duration.count() / 60 << ":" << duration.count() % 60;
            elapsedTime = oss.str();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        } });

    searchThread.detach();
    timerThread.detach();
}
