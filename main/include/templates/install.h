#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef TEMPLATE_INSTALL_H
#define TEMPLATE_INSTALL_H

namespace VortexMaker {

  VORTEX_API bool CheckTemplateInDirectory(const std::string &directory);
  VORTEX_API void InstallTemplateOnSystem(
      const std::string &directory,
      const std::string &destpath);                                                   // From anywhere to system
  VORTEX_API void InstallTemplate(const std::string &name, const std::string &path);  // From system to project
  VORTEX_API std::vector<std::shared_ptr<TemplateInterface>> FindTemplatesInDirectory(const std::string &directory);
  VORTEX_API void FindTemplatesInDirectoryRecursively(
      const std::string &directory,
      std::vector<std::shared_ptr<TemplateInterface>> &templates,
      std::atomic<bool> &stillSearching,
      std::string &elapsedTime);
}  // namespace VortexMaker

#endif  // TEMPLATE_INSTALL_H