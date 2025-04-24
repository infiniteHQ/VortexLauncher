#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef VERSION_MANAGER_WINDOW_H
#define VERSION_MANAGER_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher {
  // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

  struct VersionManagerChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    VersionManagerChild(
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined")
        : RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

  class VersionManager : public std::enable_shared_from_this<VersionManager> {
   public:
    VersionManager(const std::string &name);

    void AddChild(const std::string &child_name, const VersionManagerChild &child);
    void RemoveChild(const std::string &child_name);
    VersionManagerChild *GetChild(const std::string &child_name);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<VersionManager> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();
    void ModulesRender();

    std::unordered_map<std::string, VersionManagerChild> m_Childs;

    std::function<void()> m_CreateProjectCallback;
    std::function<void()> m_OpenProjectCallback;
    std::function<void()> m_SettingsCallback;
    std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

    std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
        const std::vector<std::shared_ptr<EnvProject>> &projects,
        size_t maxCount);
    std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
    std::string m_SelectedChildName;

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    int selected;
    float leftPaneWidth = 290.0f;
  };
}  // namespace VortexLauncher

#endif  // VERSION_MANAGER_WINDOW_H