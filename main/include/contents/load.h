#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef CONTENTS_LOAD_H
#define CONTENTS_LOAD_H

namespace VortexMaker {
  VORTEX_API void LoadSystemContents(std::vector<std::shared_ptr<ContentInterface>> &sys_contents);
  // VORTEX_API void LoadEditorModules(const std::string &directory, std::vector<void *> &plugin_handles,
  // std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}  // namespace VortexMaker

#endif  // CONTENTS_LOAD_H