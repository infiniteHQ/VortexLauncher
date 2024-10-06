#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/modules/load.h"

namespace VortexMaker
{
    /**
     * @brief Load modules from the specified directory.
     *
     * This function searches for module files in the specified directory and loads them dynamically.
     * It loads the module shared objects (SO files) and creates instances of modules using their factory functions.
     *
     * @param directory The directory where module files are located.
     * @param modules_handlers A vector to store handles of loaded modules.
     * @param modules A vector to store instances of loaded modules.
     */
    VORTEX_API void LoadEditorModules(const std::string &directory, std::vector<void *> &modules_handlers, std::vector<std::shared_ptr<ModuleInterface>> &modules)
    {
        // Search for module files recursively in the directory
        auto module_files = VortexMaker::SearchFiles(directory, "module.json");

        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                // Get the directory path containing the module file
                std::string path = file.substr(0, file.find_last_of("/"));

                // Search for shared object (SO) files in the same directory as the module configuration file
                auto so_files = VortexMaker::SearchFiles(path, "libmodule.so");

                // Iterate through each found SO file
                for (const auto &so_file : so_files)
                {
                    // Load the shared object
                    void *handle = dlopen(so_file.c_str(), RTLD_LAZY | RTLD_GLOBAL);
                    const char *dlopen_error = dlerror();
                    if (!handle)
                    {
                        // Print error if failed to load the shared object
                        std::string output = "Failed to load module: ";
                        output += dlopen_error;
                        VortexMaker::LogFatal("Modules", output); // core dumped
                        continue;
                    }

                    // Reset errors
                    dlerror();

                    // Get the symbol for creating the module instance
                    auto create_em = reinterpret_cast<ModuleInterface *(*)()>(dlsym(handle, "create_em"));
                    const char *dlsym_error = dlerror();
                    if (dlsym_error)
                    {
                        // Print error if failed to load symbol
                        std::string output = "Failed to load symbol: ";
                        output += dlsym_error;
                        VortexMaker::LogFatal("Modules", output);
                        dlclose(handle);
                        continue;
                    }

                    // Create an instance of the module
                    std::shared_ptr<ModuleInterface> new_module(create_em());

                    // Try to fetch module informations from module.json
                    try
                    {
                        new_module->m_name = json_data["name"].get<std::string>();
                        new_module->m_auto_exec = json_data["auto_exec"].get<bool>();
                        new_module->m_proper_name = json_data["proper_name"].get<std::string>();
                        new_module->m_type = json_data["type"].get<std::string>();
                        new_module->m_version = json_data["version"].get<std::string>();
                        new_module->m_description = json_data["description"].get<std::string>();
                        new_module->m_picture = json_data["picture"].get<std::string>();
                        new_module->m_logo_path = path + "/" + new_module->m_picture;
                        new_module->m_author = json_data["author"].get<std::string>();
                        new_module->m_group = json_data["group"].get<std::string>();
                        new_module->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

                        for (auto dep : json_data["deps"].get<std::vector<nlohmann::json>>())
                        {
                            std::shared_ptr<ModuleInterfaceDep> dependence = std::make_shared<ModuleInterfaceDep>();
                            dependence->name = dep["name"].get<std::string>();
                            dependence->type = dep["type"].get<std::string>();
                            for (auto version : dep["versions"])
                            {
                                dependence->supported_versions.push_back(version);
                            }
                            new_module->m_dependencies.push_back(dependence);
                        }

                        for (auto compat : json_data["compatibility"].get<std::vector<nlohmann::json>>())
                        {
                            new_module->m_supported_versions.push_back(compat.get<std::string>());
                        }
                    }
                    catch (std::exception e)
                    {
                        std::cerr << e.what() << std::endl;
                    }

                    if (!new_module)
                    {
                        // Print error if failed to create module instance
                        std::cerr << "Failed to create module instance" << std::endl;
                        dlclose(handle);
                        continue;
                    }

                    VXINFO("Modules", new_module->m_name + " module loaded with success !")

                    // Store the module instance and handle
                    modules.push_back(new_module);
                    modules_handlers.push_back(handle);
                }
            }
            catch (const std::exception &e)
            {
                // Print error if an exception occurs
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

    VORTEX_API void LoadSystemModules(std::vector<std::shared_ptr<ModuleInterface>> &sys_modules)
    {
        // Get the home directory
        std::string homeDir = VortexMaker::getHomeDirectory();

        // Module path on the system
        std::string path = homeDir + "/.vx/modules";

        // Search for module files recursively in the directory
        auto module_files = VortexMaker::SearchSystemFiles(path, "module.json");

        // Clear system modules vector
        sys_modules.clear();

        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                std::string module_path = file.substr(0, file.find_last_of("/"));

                std::shared_ptr<ModuleInterface> new_module = std::make_shared<ModuleInterface>();

                // Try to fetch module informations from module.json
                try
                {
                    new_module->m_name = json_data["name"].get<std::string>();
                    new_module->m_auto_exec = json_data["auto_exec"].get<bool>();
                    new_module->m_proper_name = json_data["proper_name"].get<std::string>();
                    new_module->m_type = json_data["type"].get<std::string>();
                    new_module->m_version = json_data["version"].get<std::string>();
                    new_module->m_description = json_data["description"].get<std::string>();
                    new_module->m_picture = json_data["picture"].get<std::string>();
                    new_module->m_logo_path = module_path + "/" + new_module->m_picture;
                    new_module->m_path = module_path + "/";
                    new_module->m_author = json_data["author"].get<std::string>();
                    new_module->m_group = json_data["group"].get<std::string>();
                    new_module->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

                    for (auto dep : json_data["deps"])
                    {
                        std::shared_ptr<ModuleInterfaceDep> dependence = std::make_shared<ModuleInterfaceDep>();
                        dependence->name = dep["name"].get<std::string>();
                        dependence->type = dep["type"].get<std::string>();
                        for (auto version : dep["versions"])
                        {
                            dependence->supported_versions.push_back(version);
                        }
                        new_module->m_dependencies.push_back(dependence);
                    }

                    new_module->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();
                }
                catch (std::exception e)
                {
                    std::cerr << e.what() << std::endl;
                }

                VXINFO("Modules registered in system ", new_module->m_name + " loaded with success !")

                // Store the module instance and handle
                sys_modules.push_back(new_module);
            }
            catch (const std::exception &e)
            {
                // Print error if an exception occurs
                VortexMaker::LogError("Core", 'Error: ' + e.what());
            }
        }
    }

} // namespace VortexMaker