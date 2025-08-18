#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"
#include "../../instances/asset_finder/asset_finder.hpp"
#include "../../utils.hpp"
namespace VortexLauncher {
  // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

  struct WelcomeWindowChild {
    std::function<void()> RenderCallback;
    std::string WebLink;
    std::string LogoPath;
    WelcomeWindowChild(
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined",
        const std::string &weblink = "undefined")
        : RenderCallback(rendercallback),
          LogoPath(logopath),
          WebLink(weblink) { };
  };

  class WelcomeWindow : public std::enable_shared_from_this<WelcomeWindow> {
   public:
    WelcomeWindow(const std::string &name);

    void AddChild(const std::string &child_name, const WelcomeWindowChild &child);
    void RemoveChild(const std::string &child_name);
    WelcomeWindowChild *GetChild(const std::string &child_name);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<WelcomeWindow> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();

    void WelcomeRender();
    void OpenProjectRender();

    std::unordered_map<std::string, WelcomeWindowChild> m_Childs;

    std::function<void()> m_CreateProjectCallback;
    std::function<void()> m_OpenProjectCallback;
    std::function<void()> m_SettingsCallback;
    std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

    std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
        const std::vector<std::shared_ptr<EnvProject>> &projects,
        size_t maxCount);
    std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
    std::string m_SelectedChildName;

    std::shared_ptr<AssetFinder> m_AssetFinder;

    std::shared_ptr<EnvProject> m_SelectedEnvproject;
    std::shared_ptr<EnvProject> m_SelectedEnvprojectToRemove;

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    int selected;
    float leftPaneWidth = 290.0f;
    float middlePaneWidth = 500.0f;
  };
}  // namespace VortexLauncher

#endif  // WELCOME_WINDOW_H