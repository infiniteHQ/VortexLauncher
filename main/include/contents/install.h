#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef CONTENT_INSTALL_H
#define CONTENT_INSTALL_H

namespace VortexMaker {

  VORTEX_API bool CheckContentInDirectory(const std::string &directory);
  VORTEX_API void InstallContentOnSystem(
      const std::string &directory,
      const std::string &destpath);                                                  // From anywhere to system
  VORTEX_API void InstallContent(const std::string &name, const std::string &path);  // From system to project
  VORTEX_API std::vector<std::shared_ptr<ContentInterface>> FindContentsInDirectory(const std::string &directory);
  VORTEX_API void FindContentsInDirectoryRecursively(
      const std::string &directory,
      std::vector<std::shared_ptr<ContentInterface>> &contents,
      std::atomic<bool> &stillSearching,
      std::string &elapsedTime);
}  // namespace VortexMaker

#endif  // CONTENT_INSTALL_H