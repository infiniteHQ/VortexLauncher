#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher
{
    // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.
    class AboutAppWindow : public std::enable_shared_from_this<AboutAppWindow>
    {
    public:
        AboutAppWindow(const std::string &name);

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<AboutAppWindow> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
}

#endif // ABOUT_WINDOW_H