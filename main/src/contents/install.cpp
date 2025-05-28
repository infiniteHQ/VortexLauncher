#include "../../include/contents/install.h"

VORTEX_API bool VortexMaker::CheckContentInDirectory(const std::string &directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    VortexMaker::LogError("Core", "Directory does not exist or is not accessible: " + directory);
    return false;
  }

  std::filesystem::path modulePath = std::filesystem::path(directory) / ".vxcontent";
  return std::filesystem::exists(modulePath) && std::filesystem::is_regular_file(modulePath);
}

VORTEX_API void VortexMaker::InstallContentOnSystem(const std::string &directory, const std::string &destpath) {
  std::string contents_path = destpath;
  std::string json_file = directory + "/.vxcontent";

  // Verify if the module is valid
  try {
    // Load JSON data from the module configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string contentid = json_data["contentid"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    contents_path += "/" + name + "." + contentid;

    VortexMaker::LogInfo("Core", "Installing the content " + name + "...");

    // Create directory
    {
      std::string cmd = "mkdir " + contents_path;
      system(cmd.c_str());
    }

    // Move the module in the final system path
    {
      std::string cmd = "cp -r " + directory + "/ " + contents_path;
      system(cmd.c_str());
    }

    VortexMaker::LogInfo("Core", "The content " + name + "is now installed on system !");
    return;
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Core", e.what());
  }

  VortexMaker::LogError("Core", "Cannot find content registered at" + directory + " !");
}

VORTEX_API void VortexMaker::InstallContent(const std::string &name, const std::string &path) {
  // TODO : Copy to project
}

VORTEX_API std::vector<std::shared_ptr<ContentInterface>> VortexMaker::FindContentsInDirectory(
    const std::string &directory) {
  // Search contents registered
  auto content_files = VortexMaker::SearchFiles(directory, ".vxcontent", 3);

  std::vector<std::shared_ptr<ContentInterface>> interfaces;

  // Iterate through each found module file
  for (const auto &file : content_files) {
    try {
      // Load JSON data from the module configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string content_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<ContentInterface> new_content = std::make_shared<ContentInterface>();

      new_content->m_name = json_data["name"].get<std::string>();
      new_content->m_proper_name = json_data["proper_name"].get<std::string>();
      new_content->m_type = json_data["type"].get<std::string>();
      new_content->m_description = json_data["description"].get<std::string>();
      new_content->m_contentbrowsercolor = json_data["contentbrowsercolor"].get<std::string>();
      new_content->m_contentid = json_data["contentid"].get<std::string>();
      new_content->m_path = content_path + "/";

      interfaces.push_back(new_content);
    } catch (std::exception e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  return interfaces;
}

VORTEX_API void VortexMaker::FindContentsInDirectoryRecursively(
    const std::string &directory,
    std::vector<std::shared_ptr<ContentInterface>> &contents,
    std::atomic<bool> &stillSearching,
    std::string &elapsedTime) {
  std::thread searchThread([directory, &contents, &stillSearching]() {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      VortexMaker::LogError("Core", "Directory does not exist or is not a directory: " + directory);
      stillSearching = false;
      return;
    }

    try {
      for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (!stillSearching)
          return;

        if (entry.is_regular_file() && entry.path().filename() == ".vxcontent") {
          try {
            // Load JSON data from the content configuration file
            auto json_data = VortexMaker::DumpJSON(entry.path().string());

            std::string content_path = entry.path().parent_path().string();

            std::shared_ptr<ContentInterface> new_content = std::make_shared<ContentInterface>();

            new_content->m_name = json_data["name"].get<std::string>();
            new_content->m_proper_name = json_data["proper_name"].get<std::string>();
            new_content->m_type = json_data["type"].get<std::string>();
            new_content->m_description = json_data["description"].get<std::string>();
            new_content->m_contentbrowsercolor = json_data["contentbrowsercolor"].get<std::string>();
            new_content->m_contentid = json_data["contentid"].get<std::string>();
            new_content->m_path = content_path + "/";

            contents.push_back(new_content);
            VortexMaker::LogInfo("Core", "Module added: " + new_content->m_name);
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
