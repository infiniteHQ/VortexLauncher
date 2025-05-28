#include "../../include/plugins/load.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

#ifdef _WIN32
#include <windows.h>
#define dlopen(lib, flags)    LoadLibrary(lib)
#define dlsym(handle, symbol) GetProcAddress((HMODULE)handle, symbol)
#define dlclose(handle)       FreeLibrary((HMODULE)handle)
#define dlerror()             "Error handled by GetLastError()"
#else
#include <dlfcn.h>
#endif

#ifdef _WIN32
#include <windows.h>

#include <string>

// Fonction utilitaire pour convertir une chaîne UTF-8 (const char*) en wide string (LPCWSTR)
std::wstring ConvertToWideString(const std::string &narrow_string) {
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, nullptr, 0);
  if (size_needed == 0) {
    throw std::runtime_error("Failed to convert string to wide string");
  }
  std::wstring wide_string(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, &wide_string[0], size_needed);
  return wide_string;
}
#endif

namespace VortexMaker {
  /**
   * @brief Load plugins from the specified directory.
   *
   * This function searches for plugin files in the specified directory and loads them dynamically.
   * It loads the plugin shared objects (SO files) and creates instances of plugins using their factory functions.
   *
   * @param directory The directory where plugin files are located.
   * @param plugins_handlers A vector to store handles of loaded plugins.
   * @param plugins A vector to store instances of loaded plugins.
   */

  void LoadEditorPlugins(
      const std::string &directory,
      std::vector<void *> &plugins_handlers,
      std::vector<std::shared_ptr<PluginInterface>> &plugins) {
    // Recherche des fichiers de plugin de manière récursive
    auto plugin_files = VortexMaker::SearchFiles(directory, "plugin.json");

    // Itération sur chaque fichier de configuration trouvé
    for (const auto &file : plugin_files) {
      try {
        // Chargement des données JSON depuis le fichier
        auto json_data = VortexMaker::DumpJSON(file);

        // Obtenir le chemin du répertoire contenant le fichier de configuration
        std::string path = file.substr(0, file.find_last_of("/\\"));

        // Recherche des fichiers partagés (SO ou DLL selon le système)
#ifdef _WIN32
        auto shared_files = VortexMaker::SearchFiles(path, "libplugin.dll");
#else
        auto shared_files = VortexMaker::SearchFiles(path, "libplugin.so");
#endif

        // Parcourir les fichiers partagés trouvés
        for (const auto &shared_file : shared_files) {
#ifdef _WIN32
          // Convertir le chemin en wide string pour LoadLibraryW
          std::wstring wide_shared_file = ConvertToWideString(shared_file);
          void *handle = reinterpret_cast<void *>(LoadLibraryW(wide_shared_file.c_str()));
#else
          void *handle = dlopen(
              shared_file.c_str(),
              RTLD_LAZY
#ifdef RTLD_GLOBAL
                  | RTLD_GLOBAL
#endif
          );
#endif
          if (!handle) {
#ifdef _WIN32
            DWORD error_code = GetLastError();
            std::cerr << "Failed to load plugin: Error Code " << error_code << std::endl;
            VortexMaker::LogFatal("plugins", "Failed to load plugin: Error Code " + std::to_string(error_code));
#else
            const char *dlopen_error = dlerror();
            std::cerr << "Failed to load plugin: " << dlopen_error << std::endl;
            VortexMaker::LogFatal("plugins", "Failed to load plugin: " + std::string(dlopen_error));
#endif
            continue;
          }

          // Réinitialisation des erreurs
#ifdef _WIN32
          SetLastError(0);
#else
          dlerror();
#endif

          // Chargement du symbole "create_ep"
          auto create_ep = reinterpret_cast<PluginInterface *(*)()>(dlsym(handle, "create_ep"));
#ifdef _WIN32
          if (!create_ep) {
            DWORD error_code = GetLastError();
            std::cerr << "Failed to load symbol: Error Code " << error_code << std::endl;
            VortexMaker::LogFatal("plugins", "Failed to load symbol: Error Code " + std::to_string(error_code));
            dlclose(handle);
            continue;
          }
#else
          const char *dlsym_error = dlerror();
          if (dlsym_error) {
            std::cerr << "Failed to load symbol: " << dlsym_error << std::endl;
            VortexMaker::LogFatal("plugins", "Failed to load symbol: " + std::string(dlsym_error));
            dlclose(handle);
            continue;
          }
#endif

          // Création de l'instance du plugin
          std::shared_ptr<PluginInterface> new_plugin(create_ep());

          try {
            // Chargement des informations du plugin depuis JSON
            new_plugin->m_name = json_data["name"].get<std::string>();
            new_plugin->m_auto_exec = json_data["auto_exec"].get<bool>();
            new_plugin->m_proper_name = json_data["proper_name"].get<std::string>();
            new_plugin->m_type = json_data["type"].get<std::string>();
            new_plugin->m_version = json_data["version"].get<std::string>();
            new_plugin->m_description = json_data["description"].get<std::string>();
            new_plugin->m_picture = json_data["picture"].get<std::string>();
            new_plugin->m_logo_path = path + "/" + new_plugin->m_picture;
            new_plugin->m_author = json_data["author"].get<std::string>();
            new_plugin->m_group = json_data["group"].get<std::string>();
            new_plugin->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

            for (auto dep : json_data["deps"].get<std::vector<nlohmann::json>>()) {
              std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
              dependence->name = dep["name"].get<std::string>();
              dependence->type = dep["type"].get<std::string>();
              for (auto version : dep["versions"]) {
                dependence->supported_versions.push_back(version);
              }
              new_plugin->m_dependencies.push_back(dependence);
            }

            for (auto compat : json_data["compatibility"].get<std::vector<nlohmann::json>>()) {
              new_plugin->m_supported_versions.push_back(compat.get<std::string>());
            }
          } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
          }

          if (!new_plugin) {
            std::cerr << "Failed to create plugin instance" << std::endl;
            dlclose(handle);
            continue;
          }

          VXINFO("Plguins", new_plugin->m_name + " plugin loaded with success!");

          // Ajouter le plugin et son gestionnaire à leurs vecteurs respectifs
          plugins.push_back(new_plugin);
          plugins_handlers.push_back(handle);
        }
      } catch (const std::exception &e) {
        // Gérer les exceptions
        std::cerr << "Error: " << e.what() << std::endl;
      }
    }
  }

  VORTEX_API void LoadSystemPlugins(std::vector<std::shared_ptr<PluginInterface>> &sys_plugins) {
    // Get the home directory
    std::string homeDir = VortexMaker::getHomeDirectory();

    VxContext &ctx = *CVortexMaker;

    ctx.IO.sys_ep.clear();

    // Clear system plugins vector
    sys_plugins.clear();

    for (const auto &pool_path : ctx.IO.sys_plugins_pools) {
      // Check if pool_path is a directory before proceeding
      if (!std::filesystem::is_directory(pool_path)) {
        continue;  // Skip to the next path
      }
      // Search for plugin files recursively in the directory
      auto plugin_files = VortexMaker::SearchSystemFiles(pool_path, "plugin.json");

      // Iterate through each found plugin file
      for (const auto &file : plugin_files) {
        try {
          // Load JSON data from the plugin configuration file
          auto json_data = VortexMaker::DumpJSON(file);

          std::string plugin_path = file.substr(0, file.find_last_of("/"));

          std::shared_ptr<PluginInterface> new_plugin = std::make_shared<PluginInterface>();

          // Try to fetch plugin informations from plugin.json
          try {
            new_plugin->m_name = json_data["name"].get<std::string>();
            new_plugin->m_auto_exec = json_data["auto_exec"].get<bool>();
            new_plugin->m_proper_name = json_data["proper_name"].get<std::string>();
            new_plugin->m_type = json_data["type"].get<std::string>();
            new_plugin->m_version = json_data["version"].get<std::string>();
            new_plugin->m_description = json_data["description"].get<std::string>();
            new_plugin->m_picture = json_data["picture"].get<std::string>();
            new_plugin->m_logo_path = plugin_path + "/" + new_plugin->m_picture;
            new_plugin->m_path = plugin_path + "/";
            new_plugin->m_author = json_data["author"].get<std::string>();
            new_plugin->m_group = json_data["group"].get<std::string>();
            new_plugin->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

            for (auto dep : json_data["deps"]) {
              std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
              dependence->name = dep["name"].get<std::string>();
              dependence->type = dep["type"].get<std::string>();
              for (auto version : dep["versions"]) {
                dependence->supported_versions.push_back(version);
              }
              new_plugin->m_dependencies.push_back(dependence);
            }

            new_plugin->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();
          } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
          }

          VXINFO("plugins registered in system ", new_plugin->m_name + " loaded with success !")

          // Store the plugin instance and handle
          sys_plugins.push_back(new_plugin);
        } catch (const std::exception &e) {
          // Print error if an exception occurs
          std::string error = e.what();
          VortexMaker::LogError("Core", "Error: " + error);
        }
      }
    }
  }

}  // namespace VortexMaker