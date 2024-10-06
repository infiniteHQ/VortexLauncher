#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef TEMPLATE_LOAD_H
#define TEMPLATE_LOAD_H

namespace VortexMaker {
    VORTEX_API void LoadSystemTemplates(std::vector<std::shared_ptr<TemplateInterface>> &sys_templates);
    //VORTEX_API void LoadEditorModules(const std::string &directory, std::vector<void *> &plugin_handles, std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}

#endif