#include "../../include/modules/install.h"

#include "../../include/modules/load.h"
#include "../../include/modules/runtime.h"

VORTEX_API void InstallModule(const std::string &module_name, const std::string &version, bool &restart_modules) {
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
  for (const auto &file : module_files) {
    try {
      // Load JSON data from the module configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string module_path = file.substr(0, file.find_last_of(VortexMaker::IsWindows() ? "\\" : "/"));

      std::string name = json_data["name"].get<std::string>();
      std::string module_version = json_data["version"].get<std::string>();

      if (name == module_name && module_version == version) {
        finded = true;

        // Move the module into the project
        {
          std::string cmd =
              VortexMaker::IsWindows()
                  ? "xcopy /E /I \"" + module_path + "\" \"" + std::string(ctx.projectPath.string()) +
                        "\\.vx\\contents\\modules\\\""
                  : "cp -r " + module_path + " " + std::string(ctx.projectPath.string()) + "/.vx/contents/modules/";

          VortexMaker::RunCommand(cmd.c_str());
        }

        // If restart_modules == true, restart all modules
        if (restart_modules) {
          // Stop all modules
          for (auto em : ctx.IO.em) {
            em->Stop();
          }

          // Clear closed modules
          ctx.IO.em.clear();

          // Load modules installed in the current project
          VortexMaker::LoadEditorModules(ctx.projectPath.string(), ctx.IO.em_handles, ctx.IO.em);

          // Finally, start all loaded modules
          VortexMaker::StartAllModules();
        }

        return;
      }
    } catch (const std::exception &e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  if (!finded) {
    VortexMaker::LogError(
        "Core", "Failed to find the module " + module_name + " with version " + version + ". Is this module installed?");
  }
}

VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> VortexMaker::FindModulesInDirectory(const std::string &directory) {
  std::vector<std::shared_ptr<ModuleInterface>> interfaces;

  try {
    // Check if the directory exists and is accessible
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
      return interfaces;  // Return an empty list if directory is not accessible
    }

    // Search modules registered
    auto module_files = VortexMaker::SearchFiles(directory, "module.json", 3);

    // Iterate through each found module file
    for (const auto &file : module_files) {
      try {
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
        for (const auto &dep : json_data["deps"]) {
          auto dependence = std::make_shared<ModuleInterfaceDep>();
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

struct ModuleIdentifier {
  std::string name;
  std::string version;
  std::string path;

  bool operator==(const ModuleIdentifier &other) const {
    return name == other.name && version == other.version && path == other.path;
  }
};

namespace std {
  template<>
  struct hash<ModuleIdentifier> {
    size_t operator()(const ModuleIdentifier &id) const {
      return hash<std::string>()(id.name + id.version + id.path);
    }
  };
}  // namespace std

VORTEX_API bool VortexMaker::CheckModuleInDirectory(const std::string &directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
    return false;
  }

  std::filesystem::path modulePath = std::filesystem::path(directory) / "module.json";
  return std::filesystem::exists(modulePath) && std::filesystem::is_regular_file(modulePath);
}

VORTEX_API std::shared_ptr<ModuleInterface> VortexMaker::FindModuleInDirectory(const std::string &directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
    return nullptr;
  }

  auto module_files = VortexMaker::SearchFiles(directory, "module.json");

  for (const auto &file : module_files) {
    try {
      auto json_data = VortexMaker::DumpJSON(file);
      if (!json_data.contains("name") || !json_data.contains("version")) {
        VortexMaker::LogError("Core", "JSON structure is missing required fields in " + file);
        continue;
      }

      auto interface = std::make_shared<ModuleInterface>();
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

VORTEX_API void VortexMaker::FindModulesInDirectoryRecursively(
    const std::string &directory,
    std::vector<std::shared_ptr<ModuleInterface>> &modules,
    std::atomic<bool> &stillSearching,
    std::string &elapsedTime) {
  std::thread searchThread([directory, &modules, &stillSearching]() {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      VortexMaker::LogError("Core", "Directory does not exist or is not a directory: " + directory);
      stillSearching = false;
      return;
    }

    try {
      for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (!stillSearching)
          return;

        if (entry.is_regular_file() && entry.path().filename() == "module.json") {
          try {
            auto json_data = VortexMaker::DumpJSON(entry.path().string());

            std::string module_path = entry.path().parent_path().string();
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

            for (const auto &dep : json_data["deps"]) {
              auto dependence = std::make_shared<ModuleInterfaceDep>();
              dependence->name = dep["name"].get<std::string>();
              dependence->type = dep["type"].get<std::string>();
              for (const auto &version : dep["versions"]) {
                dependence->supported_versions.push_back(version);
              }
              interface->m_dependencies.push_back(dependence);
            }

            interface->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();

            modules.push_back(interface);
            VortexMaker::LogInfo("Core", "Module added: " + interface->m_name);
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
