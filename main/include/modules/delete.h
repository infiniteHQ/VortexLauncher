#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef MODULE_DELETE_H
#define MODULE_DELETE_H

// TODO : Delete from hash ? (of name + version + path)

namespace VortexMaker {
  VORTEX_API void DeleteSystemModule(const std::string& name, const std::string& version);
}

#endif