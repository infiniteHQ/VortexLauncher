#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef MODULE_DUMMYFUNCTION_H
#define MODULE_DUMMYFUNCTION_H

class ModuleDummyFunction
{
public:
    ModuleDummyFunction(void(*foo)(), const std::string& name);
    virtual void execute() {};

    template<typename T>
    std::string GetFunctionName(T* functionPtr);

    std::string m_name;
    void(*m_foo)();
};

#endif