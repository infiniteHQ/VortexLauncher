#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_EVENT_H
#define PLUGIN_EVENT_H

enum class PlugDevFlag {
  DEPRECIATED,
  READY,
};

enum class PlugHappeningState {
  INFO,
  WARNING,
  ERR,
  FATAL,
};
struct PluginInputEventHappening {
  std::string m_trigger_name;
  PlugHappeningState m_state;
  std::string m_log;
  std::string m_timestamp;
};

/**
 * @brief Executed by other with custom parameters
 */
class PluginInputEvent {
 public:
  PluginInputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name);

  virtual void execute() { };

  void trigger_happening(const std::string& trigger_name, PlugHappeningState state, const std::string& log);

  void (*m_foo)(const std::shared_ptr<hArgs>& args);
  std::string m_name;
  std::string m_description;
  std::vector<std::tuple<std::string, std::string, std::string>> m_params;
  bool m_can_callback;
  PlugDevFlag m_devflag;
  std::vector<std::shared_ptr<PluginInputEventHappening>> m_happenings;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class PluginOutputEvent {
 public:
  PluginOutputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name);
  virtual void execute() { };

  void (*m_foo)(const std::shared_ptr<hArgs>& args);
  std::string m_name;
};

#endif  // PLUGIN_EVENT_H