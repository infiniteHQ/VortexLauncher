#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef PLUGIN_LOAD_H
#define PLUGIN_LOAD_H

namespace VortexMaker {
  VORTEX_API void LoadSystemPlugins(std::vector<std::shared_ptr<PluginInterface>> &sys_plugins);
  VORTEX_API void LoadEditorPlugins(
      const std::string &directory,
      std::vector<void *> &plugin_handles,
      std::vector<std::shared_ptr<PluginInterface>> &plugins);
}  // namespace VortexMaker

#endif  // PLUGIN_LOAD_H