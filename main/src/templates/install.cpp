#include "../../include/templates/install.h"

VORTEX_API bool VortexMaker::CheckTemplateInDirectory(const std::string &directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
    return false;
  }

  std::filesystem::path modulePath = std::filesystem::path(directory) / "template.json";
  return std::filesystem::exists(modulePath) && std::filesystem::is_regular_file(modulePath);
}

VORTEX_API void VortexMaker::InstallTemplateOnSystem(const std::string &directory, const std::string &destpath) {
  std::string templates_path = destpath;
  std::string json_file = directory + "/template.json";

  // Verify if the module is valid
  try {
    // Load JSON data from the module configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    templates_path += "/" + name + "." + version;

    VortexMaker::LogInfo("Core", "Installing the template " + name + "...");

    // Create directory
    {
      std::string cmd = "mkdir " + templates_path;
      system(cmd.c_str());
    }

    // Move the module in the final system path
    {
      std::string cmd = "cp -r " + directory + "/* " + templates_path;
      system(cmd.c_str());
    }

    VortexMaker::LogInfo("Core", "The template " + name + "is now installed on system !");
    return;
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Core", e.what());
  }

  VortexMaker::LogError("Core", "Cannot find template registered at" + directory + " !");
}

VORTEX_API void VortexMaker::InstallTemplate(const std::string &name, const std::string &path) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Finded state
  bool finded = false;

  // TODO : Search templates registered in project first (if the project have include_system_templates == false)
  // Search templates registered in project first
  /*...*/

  for (auto tem : ctx.IO.sys_templates) {
    if (tem->m_name == name) {
      VortexMaker::LogInfo("Core", "Installing the template \"" + name + "\" ...");

#ifdef _WIN32
      fs::create_directories(path);
#else
      {
        std::string cmd = "mkdir \"" + path + "\"";
        system(cmd.c_str());
      }
#endif

#ifdef _WIN32

      auto sanitize_path = [](std::string s) -> std::string {
        std::replace(s.begin(), s.end(), '\\', '/');
        if (!s.empty() && s.front() == '"')
          s.erase(0, 1);
        if (!s.empty() && s.back() == '"')
          s.pop_back();
        return s;
      };

      std::string archive = sanitize_path(tem->m_path + tem->m_tarball);
      std::string dest = sanitize_path(path);

      std::string cmd = "tar -xf \"" + archive + "\" --strip-components=1 -C \"" + dest + "\"";

      int result = VortexMaker::RunCommand(cmd.c_str());

      if (result != 0) {
        VortexMaker::LogError("Core", "Extraction failed with exit code: " + std::to_string(result));
      }
#else
      {
        std::string cmd = "tar -xvf \"" + tem->m_path + tem->m_tarball + "\" --strip-components=1 -C \"" + path + "\"";
        system(cmd.c_str());
      }
#endif
    }
  }

  /*// Search the template registered in the system environment (if not findeed in the system and if the project can see env
  template) if (!finded && ctx.include_system_templates)
  {
      // Get the home directory
      std::string homeDir = VortexMaker::getHomeDirectory();

      // Module path on the system
      std::string path = homeDir + "/.vx/templates";

      // Search templates registered in system
      auto module_files = VortexMaker::SearchFiles(path, "template.json");

      // Iterate through each found module file
      for (const auto &file : module_files)
      {
          try
          {
              // Load JSON data from the module configuration file
              auto json_data = VortexMaker::DumpJSON(file);

              std::string template_path = file.substr(0, file.find_last_of("/"));

              std::string name = json_data["name"].get<std::string>();

              if (name == name)
              {
                  finded = true;

                  VortexMaker::LogInfo("Core", "Installing the template \"" + name + "\" ...");

                  // TODO : Call Deploy function of the template

                  std::string cmd = "tar -xvf " + template_path +

                  // Get the target directory of installation (path)
                  // Take the tarball, and uncompress it into destination

                  return;
              }
          }
          catch (std::exception e)
          {
              VortexMaker::LogError("Core", e.what());
          }
      }
  }

  if (!finded)
  {
      VortexMaker::LogError("Core", "Failed to find the template " + name + " this template is installed ?");
  }*/
}

VORTEX_API std::vector<std::shared_ptr<TemplateInterface>> VortexMaker::FindTemplatesInDirectory(
    const std::string &directory) {
  // Search templates registered
  auto template_files = VortexMaker::SearchFiles(directory, "template.json", 3);

  std::vector<std::shared_ptr<TemplateInterface>> interfaces;

  // Iterate through each found module file
  for (const auto &file : template_files) {
    try {
      // Load JSON data from the module configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string template_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<TemplateInterface> new_template = std::make_shared<TemplateInterface>();

      new_template->m_name = json_data["name"].get<std::string>();
      new_template->m_proper_name = json_data["proper_name"].get<std::string>();
      new_template->m_type = json_data["type"].get<std::string>();
      new_template->m_description = json_data["description"].get<std::string>();
      new_template->m_picture = json_data["picture"].get<std::string>();
      new_template->m_logo_path = template_path + "/" + new_template->m_picture;
      new_template->m_path = template_path + "/";
      new_template->m_author = json_data["author"].get<std::string>();
      new_template->m_group = json_data["group"].get<std::string>();
      new_template->m_tarball = json_data["tarball"].get<std::string>();
      new_template->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

      for (auto dep : json_data["deps"]) {
        std::shared_ptr<TemplateDep> dependence = std::make_shared<TemplateDep>();
        dependence->name = dep["name"].get<std::string>();
        dependence->type = dep["type"].get<std::string>();
        for (auto version : dep["versions"]) {
          dependence->supported_versions.push_back(version);
        }
        new_template->m_dependencies.push_back(dependence);
      }

      interfaces.push_back(new_template);
    } catch (std::exception e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  return interfaces;
}

VORTEX_API void VortexMaker::FindTemplatesInDirectoryRecursively(
    const std::string &directory,
    std::vector<std::shared_ptr<TemplateInterface>> &templates,
    std::atomic<bool> &stillSearching,
    std::string &elapsedTime) {
  std::thread searchThread([directory, &templates, &stillSearching]() {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      VortexMaker::LogError("Core", "Directory does not exist or is not a directory: " + directory);
      stillSearching = false;
      return;
    }

    try {
      for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (!stillSearching)
          return;

        if (entry.is_regular_file() && entry.path().filename() == "template.json") {
          try {
            // Load JSON data from the template configuration file
            auto json_data = VortexMaker::DumpJSON(entry.path().string());

            std::string template_path = entry.path().parent_path().string();

            std::shared_ptr<TemplateInterface> new_template = std::make_shared<TemplateInterface>();

            new_template->m_name = json_data["name"].get<std::string>();
            new_template->m_proper_name = json_data["proper_name"].get<std::string>();
            new_template->m_type = json_data["type"].get<std::string>();
            new_template->m_description = json_data["description"].get<std::string>();
            new_template->m_picture = json_data["picture"].get<std::string>();
            new_template->m_logo_path = template_path + "/" + new_template->m_picture;
            new_template->m_path = template_path + "/";
            new_template->m_author = json_data["author"].get<std::string>();
            new_template->m_group = json_data["group"].get<std::string>();
            new_template->m_tarball = json_data["tarball"].get<std::string>();
            new_template->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

            for (auto dep : json_data["deps"]) {
              std::shared_ptr<TemplateDep> dependence = std::make_shared<TemplateDep>();
              dependence->name = dep["name"].get<std::string>();
              dependence->type = dep["type"].get<std::string>();
              for (auto version : dep["versions"]) {
                dependence->supported_versions.push_back(version);
              }
              new_template->m_dependencies.push_back(dependence);
            }

            templates.push_back(new_template);
            VortexMaker::LogInfo("Core", "Module added: " + new_template->m_name);
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
