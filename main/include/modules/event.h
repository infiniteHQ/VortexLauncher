#include "../vortex.h"
#include "../vortex_internals.h"


#ifndef MODULE_EVENT_H
#define MODULE_EVENT_H

enum class DevFlag
{
    DEPRECIATED,
    READY, 
};

enum class HappeningState
{
    INFO, 
    WARNING, 
    ERROR,
    FATAL,
};
struct ModuleInputEventHappening
{
    std::string m_trigger_name;
    HappeningState m_state;
    std::string m_log;
    std::string m_timestamp;
};

/**
 * @brief Executed by other with custom parameters
 */
class ModuleInputEvent
{
public:
    ModuleInputEvent(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name);
    
    virtual void execute(){};

    void trigger_happening(const std::string& trigger_name, HappeningState state, const std::string& log);

    void(*m_foo)(const std::shared_ptr<hArgs>& args);
    std::string m_name;
    std::string m_description;
    std::vector<std::tuple<std::string, std::string, std::string>> m_params;
    bool m_can_callback;
    DevFlag m_devflag;
    std::vector<std::shared_ptr<ModuleInputEventHappening>> m_happenings;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class ModuleOutputEvent
{
public:
    ModuleOutputEvent(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name);
    virtual void execute() {};

    void(*m_foo)(const std::shared_ptr<hArgs>& args);
    std::string m_name;
};

#endif