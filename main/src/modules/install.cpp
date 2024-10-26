#include "../../include/modules/install.h"
#include "../../include/modules/load.h"
#include "../../include/modules/runtime.h"

VORTEX_API void InstallModule(const std::string &module_name, const std::string &version, bool &restart_modules)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Get the home directory
    std::string homeDir = VortexMaker::getHomeDirectory();

    // Module path on the system
    std::string path = homeDir + (VortexMaker::IsWindows() ? "\\.vx\\contents\\modules" : "/.vx/contents/modules");

    // Search for registered modules
    auto module_files = VortexMaker::SearchFiles(path, "module.json");

    // Find status
    bool finded = false;

    // Iterate through each found module file
    for (const auto &file : module_files)
    {
        try
        {
            // Load JSON data from the module configuration file
            auto json_data = VortexMaker::DumpJSON(file);

            std::string module_path = file.substr(0, file.find_last_of(VortexMaker::IsWindows() ? "\\" : "/"));

            std::string name = json_data["name"].get<std::string>();
            std::string module_version = json_data["version"].get<std::string>();

            if (name == module_name && module_version == version)
            {
                finded = true;

                // Move the module into the project
                {
                    std::string cmd = VortexMaker::IsWindows() ? "xcopy /E /I \"" + module_path + "\" \"" + std::string(ctx.projectPath) + "\\.vx\\contents\\modules\\\"" : "cp -r " + module_path + " " + std::string(ctx.projectPath) + "/.vx/contents/modules/";

                    std::system(cmd.c_str());
                }

                // If restart_modules == true, restart all modules
                if (restart_modules)
                {
                    // Stop all modules
                    for (auto em : ctx.IO.em)
                    {
                        em->Stop();
                    }

                    // Clear closed modules
                    ctx.IO.em.clear();

                    // Load modules installed in the current project
                    VortexMaker::LoadEditorModules(ctx.projectPath, ctx.IO.em_handles, ctx.IO.em);

                    // Finally, start all loaded modules
                    VortexMaker::StartAllModules();
                }

                return;
            }
        }
        catch (const std::exception &e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }

    if (!finded)
    {
        VortexMaker::LogError("Core", "Failed to find the module " + module_name + " with version " + version + ". Is this module installed?");
    }
}

VORTEX_API std::shared_ptr<ModuleInterface> VortexMaker::FindModuleInDirectory(const std::string &directory)
{
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
        return nullptr;
    }

    auto module_files = VortexMaker::SearchFiles(directory, "module.json");

    for (const auto &file : module_files)
    {
        try
        {
            auto json_data = VortexMaker::DumpJSON(file);

            std::string module_path = file.substr(0, file.find_last_of(VortexMaker::IsWindows() ? '\\' : '/'));

            auto interface = std::make_shared<ModuleInterface>();
            interface->m_name = json_data["name"].get<std::string>();
            interface->m_auto_exec = json_data["auto_exec"].get<bool>();
            interface->m_proper_name = json_data["proper_name"].get<std::string>();
            interface->m_type = json_data["type"].get<std::string>();
            interface->m_version = json_data["version"].get<std::string>();
            interface->m_description = json_data["description"].get<std::string>();
            interface->m_picture = json_data["picture"].get<std::string>();
            interface->m_logo_path = module_path + "/" + interface->m_picture;
            interface->m_path = module_path + "/";
            interface->m_author = json_data["author"].get<std::string>();
            interface->m_group = json_data["group"].get<std::string>();
            interface->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

            // Handle module dependencies
            for (const auto &dep : json_data["deps"])
            {
                auto dependence = std::make_shared<ModuleInterfaceDep>();
                dependence->name = dep["name"].get<std::string>();
                dependence->type = dep["type"].get<std::string>();
                for (const auto &version : dep["versions"])
                {
                    dependence->supported_versions.push_back(version);
                }
                interface->m_dependencies.push_back(dependence);
            }

            // Set supported versions
            interface->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();

            return interface;
        }
        catch (const std::exception &e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }
    return nullptr;
}

VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> VortexMaker::FindModulesInDirectory(const std::string &directory)
{
    std::vector<std::shared_ptr<ModuleInterface>> interfaces;

    try
    {
        // Check if the directory exists and is accessible
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
        {
            VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
            return interfaces; // Return an empty list if directory is not accessible
        }

        // Search modules registered
        auto module_files = VortexMaker::SearchFiles(directory, "module.json", 3);

        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                // Determine the module path correctly based on the file separator
                std::string module_path = file.substr(0, file.find_last_of(VortexMaker::IsWindows() ? '\\' : '/'));

                // Create shared_ptr for ModuleInterface
                auto interface = std::make_shared<ModuleInterface>();

                interface->m_name = json_data["name"].get<std::string>();
                interface->m_auto_exec = json_data["auto_exec"].get<bool>();
                interface->m_proper_name = json_data["proper_name"].get<std::string>();
                interface->m_type = json_data["type"].get<std::string>();
                interface->m_version = json_data["version"].get<std::string>();
                interface->m_description = json_data["description"].get<std::string>();
                interface->m_picture = json_data["picture"].get<std::string>();
                interface->m_logo_path = module_path + "/" + interface->m_picture;
                interface->m_path = module_path + "/";
                interface->m_author = json_data["author"].get<std::string>();
                interface->m_group = json_data["group"].get<std::string>();
                interface->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

                // Handle module dependencies
                for (const auto &dep : json_data["deps"])
                {
                    auto dependence = std::make_shared<ModuleInterfaceDep>();
                    dependence->name = dep["name"].get<std::string>();
                    dependence->type = dep["type"].get<std::string>();
                    for (const auto &version : dep["versions"])
                    {
                        dependence->supported_versions.push_back(version);
                    }
                    interface->m_dependencies.push_back(dependence);
                }

                // Set supported versions
                interface->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();

                interfaces.push_back(interface);
            }
            catch (const std::exception &e) // Catch by reference to avoid slicing
            {
                VortexMaker::LogError("Core", e.what());
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e) // Catch specific filesystem errors
    {
        VortexMaker::LogError("Core", "Filesystem error: " + std::string(e.what()) + " [" + directory + "]");
    }
    catch (const std::exception &e) // Catch any other standard exceptions
    {
        VortexMaker::LogError("Core", "Unexpected error: " + std::string(e.what()));
    }

    return interfaces;
}

// Recursive function that performs the module search in the given directory
void VortexMaker::RecursiveModuleSearch(const std::string &current_directory,
                                        std::vector<std::shared_ptr<ModuleInterface>> &interfaces,
                                        std::atomic<bool> &still_searching)
{
    // Check if the search was canceled
    if (!still_searching)
    {
        VortexMaker::LogInfo("Core", "Search canceled by user.");
        return;
    }

    try
    {
        // Check if the directory exists and is accessible
        if (!std::filesystem::exists(current_directory) || !std::filesystem::is_directory(current_directory))
        {
            VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + current_directory);
            return;
        }

        // Attempt to find modules in the current directory
        auto module = VortexMaker::FindModuleInDirectory(current_directory);
        if (module)
        {
            // If a module is found, add it to the interfaces vector
            interfaces.push_back(module);
        }

        // Recursively search in subdirectories
        for (const auto &entry : std::filesystem::directory_iterator(current_directory))
        {
            if (!still_searching)
            {
                VortexMaker::LogInfo("Core", "Search canceled during subdirectory exploration.");
                return;
            }

            // Check if the entry is a directory
            if (entry.is_directory())
            {
                // Recursive call for subdirectories
                RecursiveModuleSearch(entry.path().string(), interfaces, still_searching);
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        // Handle permission errors and continue
    }
    catch (const std::exception &e)
    {
        // Handle any other exceptions and log the error
    }
}

// Function to initiate the recursive search in a detached thread
void VortexMaker::StartRecursiveModuleSearch(const std::string &directory,
                                             std::vector<std::shared_ptr<ModuleInterface>> &interfaces,
                                             std::atomic<bool> &still_searching,
                                             std::string &elapsed_time)
{
    // Capture the start time for elapsed time calculation
    auto start_time = std::chrono::steady_clock::now();

    // Launch a detached thread for the search
    std::thread([directory, &interfaces, &still_searching, &elapsed_time, start_time]()
                {
        try {
            // Start the recursive search
            RecursiveModuleSearch(directory, interfaces, still_searching);

            // Mark search as completed and calculate elapsed time
            still_searching = false;
            auto end_time = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

            // Update the elapsed_time string with the total search duration
            std::ostringstream oss;
            oss << duration << " seconds";
            elapsed_time = oss.str();

            // Log the result count and time
            VortexMaker::LogInfo("Core", std::to_string(interfaces.size()) + " modules found in " + elapsed_time);

        }
        catch (const std::exception& e) {
            VortexMaker::LogError("Core", "Unexpected error: " + std::string(e.what()));
            still_searching = false;
        } })
        .detach();
}
