#include "../../include/templates/delete.h"
#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Delete a module of the system
 *
 * @param name Name of the system module.
 * @param version Version of the system module.
 */
VORTEX_API void VortexMaker::DeleteSystemTemplate(const std::string& name, const std::string& version)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    std::vector<std::string> ff;

    for(auto sys_template : ctx.IO.sys_templates)
    {
        if(sys_template->m_name == name && sys_template->m_version == version)
        {
            {
                std::string cmd = "rm -rf " + sys_template->m_path;
                std::cout << cmd << std::endl;
                system(cmd.c_str());
            }

            VortexMaker::LogInfo("Core", "System template nammed \"" + name + "\" with version \"" + version + "\" deleted !");

            return;
        }
    }

    VortexMaker::LogError("Core", "Unable to delete a system template nammed \"" + name + "\" with version \"" + version + "\"");
}