#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#include <unordered_set>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

#ifndef MODULE_INSTALL_H
#define MODULE_INSTALL_H

namespace VortexMaker
{
    VORTEX_API void InstallModule(const std::string &directory);
    VORTEX_API std::shared_ptr<ModuleInterface> FindModuleInDirectory(const std::string &directory);
    VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> FindModulesInDirectory(const std::string &directory);

    VORTEX_API void StartModuleSearch(const std::string &root_directory, std::vector<std::shared_ptr<ModuleInterface>> &interfaces, std::atomic<bool> &still_searching, std::string &elapsed_time);
    VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> FindModulesRecursively(const std::string &root_directory, std::atomic<bool> &still_searching, std::string &elapsed_time);
}

#endif