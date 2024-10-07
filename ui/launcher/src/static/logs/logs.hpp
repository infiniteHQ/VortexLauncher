#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef LOG_UTILITY_H
#define LOG_UTILITY_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"
class LauncherLogUtility
{
public:
    LauncherLogUtility(const std::string &name);

    void RefreshRender(const std::shared_ptr<LauncherLogUtility> &instance);

    void menubar();

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
    {
        return m_AppWindow;
    }

    VxContext *ctx;

private:
    bool opened;
    std::string parent;
    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
};

#endif // LOG_UTILITY_H