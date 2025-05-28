#include "../../include/plugins/runtime.h"

VORTEX_API void FinalStartPlugin(
    const std::string &plugin_name,
    std::shared_ptr<std::vector<std::string>> processed_plugins) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  for (auto em : ctx.IO.em) {
    if (em->m_name == plugin_name) {
      if (em->m_state != "running" || em->m_state != "waiting") {
        VortexMaker::LogInfo("plugins", "Start \"" + em->m_name + "\"");
        processed_plugins->push_back(em->m_name);

        try {
          em->Start();
        } catch (const std::exception &e) {
          // Log the error
          VortexMaker::LogError("plugins", "Error starting plugin \"" + em->m_name + "\": " + e.what());
          // Continue to the next plugin
          continue;
        }
      }
    }
  }
}

VORTEX_API void VortexMaker::StartPlugin(const std::string &plugin_name) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  for (auto em : ctx.IO.em) {
    if (em->m_name == plugin_name) {
      if (em->m_state != "running") {
        VortexMaker::LogInfo("plugins", "Start \"" + em->m_name + "\"");

        std::shared_ptr<std::vector<std::string>> processed_plugins = std::make_shared<std::vector<std::string>>();

        // Start dependencies
        for (auto deps : em->m_dependencies) {
          FinalStartPlugin(deps->name, processed_plugins);
        }

        // Finally start the plugin
        em->Start();
      }
    }
  }
}

VORTEX_API void VortexMaker::StartAllPlugins() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  // TODO create dist envs
  // TODO move to (StartPlugin) function
  for (auto em : ctx.IO.em) {
    {
      // Set the plugin data path
      std::string datapath = ctx.projectPath.string();
      datapath += "/.vx/data/" + em->m_name;
      em->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    StartPlugin(em->m_name);
  }
}

VORTEX_API void VortexMaker::BootstrappAllPlugins() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  // TODO move to (StartPlugin) function
  for (auto em : ctx.IO.em) {
    {
      // Set the plugin data path
      std::string datapath = ctx.projectPath.string();
      datapath += "/.vx/data/" + em->m_name;
      em->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    if (em->m_auto_exec) {
      StartPlugin(em->m_name);
    }
  }
}