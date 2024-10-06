#include "../../include/modules/function.h"

/**
 * @brief Constructor for ModuleFunction.
 *
 * @param foo Pointer to the function.
 * @param name The name of the function.
 */
ModuleFunction::ModuleFunction(void (*foo)(), const std::string& name)
    : m_foo(foo), m_name(name) {}


ModuleFunction::ModuleFunction(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_args_foo(foo), m_name(name) {}
