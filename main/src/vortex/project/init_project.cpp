#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"
#include "../../../include/modules/load.h"
#include "../../../include/modules/runtime.h"
#include "../../../include/templates/load.h"

/**
 * @brief Initialize project settings based on provided JSON configurations.
 *
 * This function initializes project settings based on the configurations provided in a JSON object.
 * It sets various properties of the VortexMaker context object accordingly.
 *
 * @param main_configs JSON object containing main project configurations.
 */
VORTEX_API void VortexMaker::InitProject(const nlohmann::json &main_configs)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;
    // Initialize context properties based on JSON configurations
    ctx.author = main_configs["project"]["author"].get<std::string>();
    ctx.description = main_configs["project"]["description"].get<std::string>();
    ctx.label = main_configs["project"]["name"].get<std::string>();
    ctx.name = main_configs["project"]["name"].get<std::string>();
    ctx.type = main_configs["project"]["type"].get<std::string>();
    ctx.project_version = main_configs["project"]["version"].get<std::string>();
    ctx.compatibleWith = main_configs["project"]["compatibleWith"].get<std::string>();
    ctx.include_system_templates = main_configs["project"]["include_system_templates"].get<bool>();

    // Set project path to current working directory
    ctx.projectPath = fs::current_path();

    // TODO : Only for early development.
    ctx.logger = true;
    ctx.logger_registering = true;

    // Initialize environment
    VortexMaker::InitEnvironment();

    // Update projet metadata (last opened, etc...) in the ~/.vx/data/projects.json
    VortexMaker::UpdateEnvironmentProject();

    // Load modules installed in the current project
    VortexMaker::LoadEditorModules(ctx.projectPath, ctx.IO.em_handles, ctx.IO.em);

    // Load modules installed in the system
    // Note: These modules are simply initialized in the project, not loaded, but we can add these in CLI/GUI
    VortexMaker::LoadSystemModules(ctx.IO.sys_em);

    // Load templates installed in the system if the configuration allow it
    // Note: These templates are simply initialized in the project, not included, but we can add these in CLI/GUI
    if (ctx.include_system_templates)
    {
        VortexMaker::LoadSystemTemplates(ctx.IO.sys_templates);
    }

    // Finally, start all loaded modules.
    VortexMaker::BootstrappAllModules();
}