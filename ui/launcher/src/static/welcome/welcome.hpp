#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher
{
    // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.
    class WelcomeWindow : public std::enable_shared_from_this<WelcomeWindow>
    {
    public:
        WelcomeWindow(const std::string &name);

        void AddChild(const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<WelcomeWindow> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();
        void WelcomeRender();

        std::unordered_map<std::string, std::function<void()>> m_Childs;

        std::function<void()> m_CreateProjectCallback;
        std::function<void()> m_OpenProjectCallback;
        std::function<void()> m_SettingsCallback;
        std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

        std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(const std::vector<std::shared_ptr<EnvProject>> &projects, size_t maxCount);
        std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
        int selected;
        float leftPaneWidth = 300.0f;
    };
}

#endif // WELCOME_WINDOW_H