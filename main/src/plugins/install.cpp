#include "../../include/plugins/install.h"

#include "../../include/plugins/load.h"
#include "../../include/plugins/runtime.h"

VORTEX_API void InstallPlugin(const std::string &plugin_name, const std::string &version, bool &restart_plugins) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Get the home directory
  std::string homeDir = VortexMaker::getHomeDirectory();

  // plugin path on the system
  std::string path = homeDir + (VortexMaker::IsWindows() ? "\\.vx\\contents\\plugins" : "/.vx/contents/plugins");

  // Search for registered plugins
  auto plugin_files = VortexMaker::SearchFiles(path, "plugin.json");

  // Find status
  bool finded = false;

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string plugin_path = file.substr(0, file.find_last_of(VortexMaker::IsWindows() ? "\\" : "/"));

      std::string name = json_data["name"].get<std::string>();
      std::string plugin_version = json_data["version"].get<std::string>();

      if (name == plugin_name && plugin_version == version) {
        finded = true;

        // Move the plugin into the project
        {
          std::string cmd =
              VortexMaker::IsWindows()
                  ? "xcopy /E /I \"" + plugin_path + "\" \"" + std::string(ctx.projectPath.string()) +
                        "\\.vx\\contents\\plugins\\\""
                  : "cp -r " + plugin_path + " " + std::string(ctx.projectPath.string()) + "/.vx/contents/plugins/";

          std::system(cmd.c_str());
        }

        // If restart_plugins == true, restart all plugins
        if (restart_plugins) {
          // Stop all plugins
          for (auto em : ctx.IO.em) {
            em->Stop();
          }

          // Clear closed plugins
          ctx.IO.em.clear();

          // Load plugins installed in the current project
          VortexMaker::LoadEditorPlugins(ctx.projectPath.string(), ctx.IO.em_handles, ctx.IO.ep);

          // Finally, start all loaded plugins
          VortexMaker::StartAllPlugins();
        }

        return;
      }
    } catch (const std::exception &e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  if (!finded) {
    VortexMaker::LogError(
        "Core", "Failed to find the plugin " + plugin_name + " with version " + version + ". Is this plugin installed?");
  }
}

VORTEX_API std::vector<std::shared_ptr<PluginInterface>> VortexMaker::FindPluginsInDirectory(const std::string &directory) {
  std::vector<std::shared_ptr<PluginInterface>> interfaces;

  try {
    // Check if the directory exists and is accessible
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
      return interfaces;  // Return an empty list if directory is not accessible
    }

    // Search plugins registered
    auto plugin_files = VortexMaker::SearchFiles(directory, "plugin.json", 3);

    // Iterate through each found plugin file
    for (const auto &file : plugin_files) {
      try {
        // Load JSON data from the plugin configuration file
        auto json_data = VortexMaker::DumpJSON(file);

        // Determine the plugin path correctly based on the file separator
        std::string plugin_path = file.substr(0, file.find_last_of(VortexMaker::IsWindows() ? '\\' : '/'));

        // Create shared_ptr for PluginInterface
        auto interface = std::make_shared<PluginInterface>();

        interface->m_name = json_data["name"].get<std::string>();
        interface->m_auto_exec = json_data["auto_exec"].get<bool>();
        interface->m_proper_name = json_data["proper_name"].get<std::string>();
        interface->m_type = json_data["type"].get<std::string>();
        interface->m_version = json_data["version"].get<std::string>();
        interface->m_description = json_data["description"].get<std::string>();
        interface->m_picture = json_data["picture"].get<std::string>();
        interface->m_logo_path = plugin_path + "/" + interface->m_picture;
        interface->m_path = plugin_path + "/";
        interface->m_author = json_data["author"].get<std::string>();
        interface->m_group = json_data["group"].get<std::string>();
        interface->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

        // Handle plugin dependencies
        for (const auto &dep : json_data["deps"]) {
          auto dependence = std::make_shared<PluginInterfaceDep>();
          dependence->name = dep["name"].get<std::string>();
          dependence->type = dep["type"].get<std::string>();
          for (const auto &version : dep["versions"]) {
            dependence->supported_versions.push_back(version);
          }
          interface->m_dependencies.push_back(dependence);
        }

        // Set supported versions
        interface->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();

        interfaces.push_back(interface);
      } catch (const std::exception &e)  // Catch by reference to avoid slicing
      {
        VortexMaker::LogError("Core", e.what());
      }
    }
  } catch (const std::filesystem::filesystem_error &e)  // Catch specific filesystem errors
  {
    VortexMaker::LogError("Core", "Filesystem error: " + std::string(e.what()) + " [" + directory + "]");
  } catch (const std::exception &e)  // Catch any other standard exceptions
  {
    VortexMaker::LogError("Core", "Unexpected error: " + std::string(e.what()));
  }

  return interfaces;
}

struct PluginIdentifier {
  std::string name;
  std::string version;
  std::string path;

  bool operator==(const PluginIdentifier &other) const {
    return name == other.name && version == other.version && path == other.path;
  }
};

namespace std {
  template<>
  struct hash<PluginIdentifier> {
    size_t operator()(const PluginIdentifier &id) const {
      return hash<std::string>()(id.name + id.version + id.path);
    }
  };
}  // namespace std

VORTEX_API bool VortexMaker::CheckPluginInDirectory(const std::string &directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
    return false;
  }

  std::filesystem::path pluginPath = std::filesystem::path(directory) / "plugin.json";
  return std::filesystem::exists(pluginPath) && std::filesystem::is_regular_file(pluginPath);
}

VORTEX_API std::shared_ptr<PluginInterface> VortexMaker::FindPluginInDirectory(const std::string &directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
    return nullptr;
  }

  auto plugin_files = VortexMaker::SearchFiles(directory, "plugin.json");

  for (const auto &file : plugin_files) {
    try {
      auto json_data = VortexMaker::DumpJSON(file);
      if (!json_data.contains("name") || !json_data.contains("version")) {
        VortexMaker::LogError("Core", "JSON structure is missing required fields in " + file);
        continue;
      }

      auto interface = std::make_shared<PluginInterface>();
      interface->m_name = json_data["name"].get<std::string>();
      interface->m_version = json_data["version"].get<std::string>();
      interface->m_path = directory;

      // Populate other fields if needed (like description, picture, etc.)
      // ...

      return interface;
    } catch (const std::exception &e) {
      VortexMaker::LogError("Core", "Error parsing JSON in " + file + ": " + e.what());
    }
  }
  return nullptr;
}

VORTEX_API void VortexMaker::FindPluginsInDirectoryRecursively(
    const std::string &directory,
    std::vector<std::shared_ptr<PluginInterface>> &plugins,
    std::atomic<bool> &stillSearching,
    std::string &elapsedTime) {
  std::thread searchThread([directory, &plugins, &stillSearching]() {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      VortexMaker::LogError("Core", "Directory does not exist or is not a directory: " + directory);
      stillSearching = false;
      return;
    }

    try {
      for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (!stillSearching)
          return;

        if (entry.is_regular_file() && entry.path().filename() == "plugin.json") {
          try {
            auto json_data = VortexMaker::DumpJSON(entry.path().string());

            std::string plugin_path = entry.path().parent_path().string();
            auto interface = std::make_shared<PluginInterface>();

            interface->m_name = json_data["name"].get<std::string>();
            interface->m_auto_exec = json_data["auto_exec"].get<bool>();
            interface->m_proper_name = json_data["proper_name"].get<std::string>();
            interface->m_type = json_data["type"].get<std::string>();
            interface->m_version = json_data["version"].get<std::string>();
            interface->m_description = json_data["description"].get<std::string>();
            interface->m_picture = json_data["picture"].get<std::string>();
            interface->m_logo_path = plugin_path + "/" + interface->m_picture;
            interface->m_path = plugin_path + "/";
            interface->m_author = json_data["author"].get<std::string>();
            interface->m_group = json_data["group"].get<std::string>();
            interface->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

            for (const auto &dep : json_data["deps"]) {
              auto dependence = std::make_shared<PluginInterfaceDep>();
              dependence->name = dep["name"].get<std::string>();
              dependence->type = dep["type"].get<std::string>();
              for (const auto &version : dep["versions"]) {
                dependence->supported_versions.push_back(version);
              }
              interface->m_dependencies.push_back(dependence);
            }

            interface->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();

            plugins.push_back(interface);
            VortexMaker::LogInfo("Core", "Plugin added: " + interface->m_name);
          } catch (const std::exception &e) {
            VortexMaker::LogError("Core", "Error processing file " + entry.path().string() + ": " + e.what());
          }
        }
      }
    } catch (const std::filesystem::filesystem_error &e) {
      VortexMaker::LogError("Core", "Filesystem error during recursive search: " + std::string(e.what()));
    } catch (const std::exception &e) {
      VortexMaker::LogError("Core", "General error during recursive search: " + std::string(e.what()));
    }

    stillSearching = false;
  });

  std::thread timerThread([&]() {
    auto start = std::chrono::steady_clock::now();
    while (stillSearching) {
      auto now = std::chrono::steady_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start);

      std::ostringstream oss;
      oss << duration.count() / 60 << ":" << duration.count() % 60;
      elapsedTime = oss.str();

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });

  searchThread.detach();
  timerThread.detach();
}
