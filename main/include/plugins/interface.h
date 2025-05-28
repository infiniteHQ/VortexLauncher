#include <cxxabi.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "../vortex.h"
#include "dummy_function.h"
#include "event.h"
#include "function.h"
#include "render_instance.h"

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

struct PluginInterfaceDep {
  std::string type;  // em, plugin, etc..
  std::string name;
  std::vector<std::string> supported_versions;
  bool satisfied;
};

class PluginInterface {
  // Plugin API (declarations needs to be here)
 public:
  virtual ~PluginInterface() {
  }
  virtual void execute() { };
  virtual void destroy() { };
  virtual void render() { };

  void Start();
  void Stop();

  void OnInputEvent();
  void OnOutputEvent();

  void AddInputEventHandler();
  void AddOutputEventHandler();

  template<typename T>
  void AddArg(const std::string& key, T value);

  void AddLogo(const uint8_t* hexa, size_t size);
  void AddLogo(const std::string& relative_path);
  void AddFunction(void (*item)(), const std::string& name);
  void AddFunction(void (*item)(), const std::string& name, const std::string& description);
  void AddFunction(
      void (*item)(const std::shared_ptr<hArgs>& args),
      const std::string& name,
      const std::string& description,
      const std::vector<std::tuple<std::string, std::string, std::string>>& args_def,
      const bool& can_callback);

  // void AddFunction(void (*item)(), const std::string& name, Parameters params);
  void AddInputEvent(void (*item)(const std::shared_ptr<hArgs>& args), const std::string& name);
  void AddInputEvent(
      void (*item)(const std::shared_ptr<hArgs>& args),
      const std::string& name,
      PlugDevFlag devflag,
      const std::string& description,
      const std::vector<std::tuple<std::string, std::string, std::string>>& args_def,
      const bool& can_callback);
  void AddOutputEvent(void (*item)(const std::shared_ptr<hArgs>& args), const std::string& name);
  std::shared_ptr<PluginInterface> GetInterface();

  void ExecFunction(const std::string& name);
  void ExecFunction(const std::string& name, std::shared_ptr<hArgs> args);
  void ExecInputEvent(const std::string& name, std::shared_ptr<hArgs> args);

  // Trigger all output events for every Plugins/plugins
  void DeployOutputEvent(const std::string& name, std::shared_ptr<hArgs> args);
  void ExecOutputEvent(const std::string& name, std::shared_ptr<hArgs> args);

  template<typename T>
  void AddPluginItemParam(const void* item, std::pair<std::string, T> parameter);

  void AddPluginOutputEvent(const PluginOutputEvent& event);
  void AddPluginInputEvent(const PluginInputEvent& event);
  void AddPluginRenderInstance(const std::shared_ptr<PluginRenderInstance>& event);
  void AddPluginFunction(const PluginFunction& event);
  std::vector<std::shared_ptr<PluginRenderInstance>> GetPluginRenderInstances();

  void LogInfo(const std::string& message);
  void LogWarning(const std::string& message);
  void LogError(const std::string& message);
  void LogFatal(const std::string& message);
  void CallInputEvent(
      const std::shared_ptr<hArgs>& args,
      const std::string& event_name,
      const std::string& plugin_name,
      void (*callback)(std::shared_ptr<hArgs> _args));
  void CallInputEvent(const std::shared_ptr<hArgs>& args, const std::string& event_name, const std::string& plugin_name);

  static std::shared_ptr<PluginInterface> GetEditorPluginByName(const std::string& name);

  void CheckDependencies();
  void CheckVersion();

  std::shared_ptr<hArgs> m_args;
  std::string m_datapath;

 public:
  std::string m_type;
  std::string m_proper_name;
  std::string m_name;
  std::string m_version;
  std::string m_path;
  std::string m_author;
  std::string m_group;
  std::string m_picture;
  std::string m_banner_path;
  std::string m_logo_path;
  std::string m_description;
  bool m_auto_exec;
  std::vector<std::string> m_contributors;
  std::vector<std::shared_ptr<PluginInterfaceDep>> m_dependencies;
  std::vector<std::string> m_supported_versions;
  const uint8_t* m_logo;
  size_t m_logo_size;

  std::string m_state = "unknow";

  bool m_selected = false;  // Launcher only

  std::vector<std::shared_ptr<PluginFunction>> m_functions;
  std::vector<std::shared_ptr<PluginOutputEvent>> m_output_events;
  std::vector<std::shared_ptr<PluginInputEvent>> m_input_events;

 private:
  std::vector<std::shared_ptr<PluginDummyFunction>> m_dummy_functions;
  std::vector<std::shared_ptr<PluginRenderInstance>> m_render_instances;
};

#endif  // PLUGIN_INTERFACE_H