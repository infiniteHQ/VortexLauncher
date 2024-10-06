#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"


#ifndef MODULE_RUNTIME_H
#define MODULE_RUNTIME_H

namespace VortexMaker {
    VORTEX_API void StartModule(const std::string& module_name);
    VORTEX_API void StartAllModules();
    VORTEX_API void BootstrappAllModules();
}

#endif