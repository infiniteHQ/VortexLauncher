#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_DELETE_H
#define PLUGIN_DELETE_H

// TODO : Delete from hash ? (of name + version + path)

namespace VortexMaker {
  VORTEX_API void DeleteSystemPlugin(const std::string& name, const std::string& version);
}

#endif / PLUGIN_DELETE_H