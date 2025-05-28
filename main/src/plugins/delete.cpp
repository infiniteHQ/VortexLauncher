#include "../../include/plugins/delete.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Delete a plugin of the system
 *
 * @param name Name of the system plugin.
 * @param version Version of the system plugin.
 */
VORTEX_API void VortexMaker::DeleteSystemPlugin(const std::string& name, const std::string& version) {
  // Get reference to the Vortex context
  VxContext& ctx = *CVortexMaker;

  for (auto plugin : ctx.IO.sys_em) {
    if (plugin->m_name == name && plugin->m_version == version) {
      {
        std::string cmd = "rm -rf " + plugin->m_path;
        system(cmd.c_str());
      }

      VortexMaker::LogInfo("Core", "System plugin nammed \"" + name + "\" with version \"" + version + "\" deleted !");
    }
  }

  VortexMaker::LogError("Core", "Unable to delete a system plugin nammed \"" + name + "\" with version \"" + version + "\"");
}