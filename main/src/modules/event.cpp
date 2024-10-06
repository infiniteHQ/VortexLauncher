#include "../../include/modules/event.h"

/**
 * @brief Constructor for ModuleInputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
ModuleInputEvent::ModuleInputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_foo(foo), m_name(name) {}

/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void ModuleInputEvent::trigger_happening(const std::string& trigger_name, HappeningState state, const std::string& log)
{
    std::shared_ptr<ModuleInputEventHappening> new_trigger = std::make_shared<ModuleInputEventHappening>();
    new_trigger->m_trigger_name = trigger_name;
    new_trigger->m_state = state;
    new_trigger->m_log = log;
    new_trigger->m_timestamp = VortexMaker::getCurrentTimeStamp();

    if(state == HappeningState::INFO)
    {
        VortexMaker::LogInfo("Modules Events", log);
    }
    else if(state == HappeningState::WARNING)
    {
        VortexMaker::LogWarn("Modules Events", log);
    }
    else if(state == HappeningState::ERROR)
    {
        VortexMaker::LogError("Modules Events", log);
    }
    else if(state == HappeningState::FATAL)
    {
        VortexMaker::LogFatal("Modules Events", log);
    }

    this->m_happenings.push_back(new_trigger);
}

/**
 * @brief Constructor for ModuleOutputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
ModuleOutputEvent::ModuleOutputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_foo(foo), m_name(name) {}


