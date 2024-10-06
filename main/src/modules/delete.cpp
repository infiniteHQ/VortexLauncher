#include "../../include/modules/delete.h"
#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Delete a module of the system
 *
 * @param name Name of the system module.
 * @param version Version of the system module.
 */
VORTEX_API void VortexMaker::DeleteSystemModule(const std::string& name, const std::string& version)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    for(auto module : ctx.IO.sys_em)
    {
        if(module->m_name == name && module->m_version == version)
        {
            {
                std::string cmd = "rm -rf " + module->m_path;
                system(cmd.c_str());
            }

            VortexMaker::LogInfo("Core", "System module nammed \"" + name + "\" with version \"" + version + "\" deleted !");
        }
    }

    VortexMaker::LogError("Core", "Unable to delete a system module nammed \"" + name + "\" with version \"" + version + "\"");
}