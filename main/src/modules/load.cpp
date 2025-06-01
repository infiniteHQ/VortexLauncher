#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/modules/load.h"


#ifdef _WIN32
#include <windows.h>
#define dlopen(lib, flags) LoadLibrary(lib)
#define dlsym(handle, symbol) GetProcAddress((HMODULE)handle, symbol)
#define dlclose(handle) FreeLibrary((HMODULE)handle)
#define dlerror() "Error handled by GetLastError()"
#else
#include <dlfcn.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <string>

// Fonction utilitaire pour convertir une chaîne UTF-8 (const char*) en wide string (LPCWSTR)
static std::wstring ConvertToWideString(const std::string &narrow_string)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, nullptr, 0);
    if (size_needed == 0)
    {
        throw std::runtime_error("Failed to convert string to wide string");
    }
    std::wstring wide_string(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, &wide_string[0], size_needed);
    return wide_string;
}
#endif

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


void LoadEditorModules(const std::string &directory, std::vector<void *> &modules_handlers, std::vector<std::shared_ptr<ModuleInterface>> &modules)
{
    // Recherche des fichiers de module de manière récursive
    auto module_files = VortexMaker::SearchFiles(directory, "module.json");

    // Itération sur chaque fichier de configuration trouvé
    for (const auto &file : module_files)
    {
        try
        {
            // Chargement des données JSON depuis le fichier
            auto json_data = VortexMaker::DumpJSON(file);

            // Obtenir le chemin du répertoire contenant le fichier de configuration
            std::string path = file.substr(0, file.find_last_of("/\\"));

            // Recherche des fichiers partagés (SO ou DLL selon le système)
#ifdef _WIN32
            auto shared_files = VortexMaker::SearchFiles(path, "libmodule.dll");
#else
            auto shared_files = VortexMaker::SearchFiles(path, "libmodule.so");
#endif

            // Parcourir les fichiers partagés trouvés
            for (const auto &shared_file : shared_files)
            {

#ifdef _WIN32
                // Convertir le chemin en wide string pour LoadLibraryW
                std::wstring wide_shared_file = ConvertToWideString(shared_file);
                void *handle = reinterpret_cast<void *>(LoadLibraryW(wide_shared_file.c_str()));
#else
                void *handle = dlopen(shared_file.c_str(), RTLD_LAZY
#ifdef RTLD_GLOBAL
                                      | RTLD_GLOBAL
#endif
                );
#endif
                if (!handle)
                {
#ifdef _WIN32
                    DWORD error_code = GetLastError();
                    std::cerr << "Failed to load module: Error Code " << error_code << std::endl;
                    VortexMaker::LogFatal("Modules", "Failed to load module: Error Code " + std::to_string(error_code));
#else
                    const char *dlopen_error = dlerror();
                    std::cerr << "Failed to load module: " << dlopen_error << std::endl;
                    VortexMaker::LogFatal("Modules", "Failed to load module: " + std::string(dlopen_error));
#endif
                    continue;
                }

                // Réinitialisation des erreurs
#ifdef _WIN32
                SetLastError(0);
#else
                dlerror();
#endif

                // Chargement du symbole "create_em"
                auto create_em = reinterpret_cast<ModuleInterface *(*)()>(dlsym(handle, "create_em"));
#ifdef _WIN32
                if (!create_em)
                {
                    DWORD error_code = GetLastError();
                    std::cerr << "Failed to load symbol: Error Code " << error_code << std::endl;
                    VortexMaker::LogFatal("Modules", "Failed to load symbol: Error Code " + std::to_string(error_code));
                    dlclose(handle);
                    continue;
                }
#else
                const char *dlsym_error = dlerror();
                if (dlsym_error)
                {
                    std::cerr << "Failed to load symbol: " << dlsym_error << std::endl;
                    VortexMaker::LogFatal("Modules", "Failed to load symbol: " + std::string(dlsym_error));
                    dlclose(handle);
                    continue;
                }
#endif

                // Création de l'instance du module
                std::shared_ptr<ModuleInterface> new_module(create_em());

                try
                {
                    // Chargement des informations du module depuis JSON
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
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                }

                if (!new_module)
                {
                    std::cerr << "Failed to create module instance" << std::endl;
                    dlclose(handle);
                    continue;
                }

                VXINFO("Modules", new_module->m_name + " module loaded with success!");

                // Ajouter le module et son gestionnaire à leurs vecteurs respectifs
                modules.push_back(new_module);
                modules_handlers.push_back(handle);
            }
        }
        catch (const std::exception &e)
        {
            // Gérer les exceptions
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

    VORTEX_API void LoadSystemModules(std::vector<std::shared_ptr<ModuleInterface>> &sys_modules)
    {
        // Get the home directory
        std::string homeDir = VortexMaker::getHomeDirectory();

        VxContext &ctx = *CVortexMaker;

        ctx.IO.sys_em.clear();

        // Clear system modules vector
        sys_modules.clear();

        for (const auto &pool_path : ctx.IO.sys_modules_pools)
        {
            // Check if pool_path is a directory before proceeding
            if (!std::filesystem::is_directory(pool_path))
            {
                continue; // Skip to the next path
            }
            // Search for module files recursively in the directory
            auto module_files = VortexMaker::SearchSystemFiles(pool_path, "module.json");

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
                    std::string error = e.what();
                    VortexMaker::LogError("Core", "Error: " + error);
                }
            }
        }
    }

} // namespace VortexMaker