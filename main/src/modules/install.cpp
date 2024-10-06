#include "../../include/modules/install.h"
#include "../../include/modules/load.h"
#include "../../include/modules/runtime.h"

VORTEX_API void VortexMaker::InstallModule(const std::string &module_name, const std::string &version, bool &restart_modules)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Get the home directory
    std::string homeDir = VortexMaker::getHomeDirectory();

    // Module path on the system
    std::string path = homeDir + "/.vx/modules";

    // Search modules registered
    auto module_files = VortexMaker::SearchFiles(path, "module.json");

    // Finded state
    bool finded = false;

    // Iterate through each found module file
    for (const auto &file : module_files)
    {
        try
        {
            // Load JSON data from the module configuration file
            auto json_data = VortexMaker::DumpJSON(file);

            std::string module_path = file.substr(0, file.find_last_of("/"));

            std::string name = json_data["name"].get<std::string>();
            std::string version = json_data["version"].get<std::string>();

            if (name == module_name && version == version)
            {
                finded = true;

                // Move the module into the project
                {
                    std::string cmd = "cp -r " + module_path + " " + ctx.projectPath.c_str() + "/.vx/modules/";
                    system(cmd.c_str());
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

                    // Finally, start all loaded modules.
                    VortexMaker::StartAllModules();
                }

                return;
            }
        }
        catch (std::exception e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }

    if (!finded)
    {
        VortexMaker::LogError("Core", "Failed to find the module " + module_name + " with version " + version + ", this module is installed ?");
    }

    // Search module registered by module_name in env
    // Get main informations
    // Create the folder name
    // Copy distant module content into the module into the projet
    // Restart modules
}

VORTEX_API std::shared_ptr<ModuleInterface> VortexMaker::FindModuleInDirectory(const std::string &directory)
{
    // Search modules registered
    auto module_files = VortexMaker::SearchFiles(directory, "module.json");


    // Iterate through each found module file
    for (const auto &file : module_files)
    {
        try
        {
            // Load JSON data from the module configuration file
            auto json_data = VortexMaker::DumpJSON(file);

            std::string module_path = file.substr(0, file.find_last_of("/"));

            std::shared_ptr<ModuleInterface> interface = std::make_shared<ModuleInterface>();

            interface->m_name = json_data["name"].get<std::string>();
            interface->m_version = json_data["version"].get<std::string>();
            
            return interface;
            
        }
        catch (std::exception e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }

    VortexMaker::LogError("Core", "Failed to find the module at " + directory + ", no modules detected.");
    return nullptr;
}

VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> VortexMaker::FindModulesInDirectory(const std::string &directory)
{
    // Search modules registered
    auto module_files = VortexMaker::SearchFiles(directory, "module.json", 3);

    std::vector<std::shared_ptr<ModuleInterface>> interfaces;

    // Iterate through each found module file
    for (const auto &file : module_files)
    {
        try
        {
            // Load JSON data from the module configuration file
            auto json_data = VortexMaker::DumpJSON(file);

            std::string module_path = file.substr(0, file.find_last_of("/"));

            std::shared_ptr<ModuleInterface> interface = std::make_shared<ModuleInterface>();
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

                    for (auto dep : json_data["deps"])
                    {
                        std::shared_ptr<ModuleInterfaceDep> dependence = std::make_shared<ModuleInterfaceDep>();
                        dependence->name = dep["name"].get<std::string>();
                        dependence->type = dep["type"].get<std::string>();
                        for (auto version : dep["versions"])
                        {
                            dependence->supported_versions.push_back(version);
                        }
                        interface->m_dependencies.push_back(dependence);
                    }

                    interface->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();

            interfaces.push_back(interface);
            
        }
        catch (std::exception e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }

    return interfaces;
}
