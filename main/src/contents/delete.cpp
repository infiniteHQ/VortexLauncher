#include "../../include/contents/delete.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Delete a module of the system
 *
 * @param name Name of the system module.
 * @param version Version of the system module.
 */
VORTEX_API void VortexMaker::DeleteSystemContent(const std::string& name, const std::string& contentid) {
  // Get reference to the Vortex context
  VxContext& ctx = *CVortexMaker;

  std::vector<std::string> ff;

  for (auto sys_content : ctx.IO.sys_contents) {
    if (sys_content->m_name == name && sys_content->m_contentid == contentid) {
      {
        std::string cmd = "rm -rf " + sys_content->m_path;
        std::cout << cmd << std::endl;
        system(cmd.c_str());
      }

      VortexMaker::LogInfo("Core", "System template nammed \"" + name + "\"deleted !");

      return;
    }
  }

  VortexMaker::LogError("Core", "Unable to delete a system template nammed \"" + name + "\"");
}