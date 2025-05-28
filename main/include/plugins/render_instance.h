#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_RENDERINSTANCE_H
#define PLUGIN_RENDERINSTANCE_H

class PluginRenderInstance {
 public:
  virtual void render() { };
};

#endif  // PLUGIN_RENDERINSTANCE_H