#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <unordered_set>

#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_INSTALL_H
#define MODULE_INSTALL_H

namespace VortexMaker {
  VORTEX_API void InstallModule(const std::string &directory);
  VORTEX_API std::shared_ptr<ModuleInterface> FindModuleInDirectory(const std::string &directory);
  VORTEX_API bool CheckModuleInDirectory(const std::string &directory);
  VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> FindModulesInDirectory(const std::string &directory);
  VORTEX_API void FindModulesInDirectoryRecursively(
      const std::string &directory,
      std::vector<std::shared_ptr<ModuleInterface>> &modules,
      std::atomic<bool> &stillSearching,
      std::string &elapsedTime);
}  // namespace VortexMaker

#endif