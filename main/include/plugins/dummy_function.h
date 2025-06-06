#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_DUMMYFUNCTION_H
#define PLUGIN_DUMMYFUNCTION_H

class PluginDummyFunction {
 public:
  PluginDummyFunction(void (*foo)(), const std::string& name);
  virtual void execute() { };

  template<typename T>
  std::string GetFunctionName(T* functionPtr);

  std::string m_name;
  void (*m_foo)();
};

#endif  // PLUGIN_DUMMYFUNCTION_H