#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef PLUGIN_RUNTIME_H
#define PLUGIN_RUNTIME_H

namespace VortexMaker {
  VORTEX_API void StartPlugin(const std::string& plugin_name);
  VORTEX_API void StartAllPlugins();
  VORTEX_API void BootstrappAllPlugins();
}  // namespace VortexMaker

#endif  // PLUGIN_RUNTIME_H