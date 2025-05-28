#include "../../include/plugins/event.h"

/**
 * @brief Constructor for PluginInputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
PluginInputEvent::PluginInputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_foo(foo),
      m_name(name) {
}

/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void PluginInputEvent::trigger_happening(const std::string& trigger_name, PlugHappeningState state, const std::string& log) {
  std::shared_ptr<PluginInputEventHappening> new_trigger = std::make_shared<PluginInputEventHappening>();
  new_trigger->m_trigger_name = trigger_name;
  new_trigger->m_state = state;
  new_trigger->m_log = log;
  new_trigger->m_timestamp = VortexMaker::getCurrentTimeStamp();

  if (state == PlugHappeningState::INFO) {
    VortexMaker::LogInfo("Plugins Events", log);
  } else if (state == PlugHappeningState::WARNING) {
    VortexMaker::LogWarn("Plugins Events", log);
  } else if (state == PlugHappeningState::ERR) {
    VortexMaker::LogError("Plugins Events", log);
  } else if (state == PlugHappeningState::FATAL) {
    VortexMaker::LogFatal("Plugins Events", log);
  }

  this->m_happenings.push_back(new_trigger);
}

/**
 * @brief Constructor for PluginOutputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
PluginOutputEvent::PluginOutputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_foo(foo),
      m_name(name) {
}
