#include "../../include/contents/load.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

namespace VortexMaker {
  VORTEX_API void LoadSystemContents(std::vector<std::shared_ptr<ContentInterface>> &sys_contents) {
    // Get the home directory
    std::string homeDir = VortexMaker::getHomeDirectory();

    VxContext &ctx = *CVortexMaker;

    ctx.IO.sys_contents.clear();

    // Clear actual contents array
    sys_contents.clear();

    for (const auto &pool_path : ctx.IO.sys_contents_pools) {
      // Check if pool_path is a directory before proceeding
      if (!std::filesystem::is_directory(pool_path)) {
        continue;  // Skip to the next path
      }

      // Search for module files recursively in the directory
      auto module_files = VortexMaker::SearchSystemFiles(pool_path, ".vxcontent");

      // Iterate through each found module file
      for (const auto &file : module_files) {
        try {
          // Load JSON data from the module configuration file
          auto json_data = VortexMaker::DumpJSON(file);

          std::string module_path = file.substr(0, file.find_last_of("/"));

          std::shared_ptr<ContentInterface> new_content = std::make_shared<ContentInterface>();

          // Try to fetch module informations from module.json
          try {
            new_content->m_name = json_data["name"].get<std::string>();
            new_content->m_proper_name = json_data["proper_name"].get<std::string>();
            new_content->m_type = json_data["type"].get<std::string>();
            new_content->m_description = json_data["description"].get<std::string>();
            new_content->m_contentbrowsercolor = json_data["contentbrowsercolor"].get<std::string>();
            new_content->m_contentid = json_data["contentid"].get<std::string>();
            new_content->m_path = module_path + "/";

          } catch (std::exception e) {
            VortexMaker::LogError("Core", e.what());
          }

          VXINFO("content registered in system ", new_content->m_name + " loaded with success !")

          // Store the module instance and handle
          sys_contents.push_back(new_content);
        } catch (const std::exception &e) {
          // Print error if an exception occurs
          std::cerr << "Error: " << e.what() << std::endl;
        }
      }
    }
  }

}  // namespace VortexMaker