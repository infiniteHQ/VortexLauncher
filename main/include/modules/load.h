#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_LOAD_H
#define MODULE_LOAD_H

namespace VortexMaker {
    VORTEX_API void LoadSystemModules(std::vector<std::shared_ptr<ModuleInterface>> &sys_modules);
    VORTEX_API void LoadEditorModules(const std::string &directory, std::vector<void *> &plugin_handles, std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}

#endif