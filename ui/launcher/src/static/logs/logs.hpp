#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef LOG_UTILITY_H
#define LOG_UTILITY_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher
{

    class LauncherLogUtility : public std::enable_shared_from_this<LauncherLogUtility>
    {
    public:
        LauncherLogUtility(const std::string &name);

        void menubar();

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<LauncherLogUtility> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        VxContext *ctx;

    private:
        bool opened;
        std::string parent;
        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
} // namespace VortexLauncher

#endif // LOG_UTILITY_H