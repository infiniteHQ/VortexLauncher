#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_FUNCTION_H
#define PLUGIN_FUNCTION_H

class PluginFunction {
 public:
  PluginFunction(void (*foo)(), const std::string& name);
  PluginFunction(void (*m_args_foo)(const std::shared_ptr<hArgs>& args), const std::string& name);

  virtual void execute() { };

  void (*m_foo)();
  void (*m_args_foo)(const std::shared_ptr<hArgs>& args);
  std::string m_name;
  std::string m_description;
  bool m_can_callback;
  std::shared_ptr<hArgs> m_args;
  std::vector<std::tuple<std::string, std::string, std::string>> m_params_def;
};

#endif  // PLUGIN_FUNCTION_H