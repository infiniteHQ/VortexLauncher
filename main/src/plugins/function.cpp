#include "../../include/plugins/function.h"

/**
 * @brief Constructor for PluginFunction.
 *
 * @param foo Pointer to the function.
 * @param name The name of the function.
 */
PluginFunction::PluginFunction(void (*foo)(), const std::string& name) : m_foo(foo), m_name(name) {
}

PluginFunction::PluginFunction(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_args_foo(foo),
      m_name(name) {
}
