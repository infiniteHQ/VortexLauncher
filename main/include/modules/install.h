#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"


#ifndef MODULE_INSTALL_H
#define MODULE_INSTALL_H

namespace VortexMaker {
    VORTEX_API void InstallModule(const std::string &directory);
    VORTEX_API std::shared_ptr<ModuleInterface> FindModuleInDirectory(const std::string &directory);
    VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> FindModulesInDirectory(const std::string &directory);
}

#endif