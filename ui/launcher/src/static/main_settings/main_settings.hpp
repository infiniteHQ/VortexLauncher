#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef MAIN_SETTINGS_WINDOW_H
#define MAIN_SETTINGS_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher {
  // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

  struct MainSettingsChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    MainSettingsChild(
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined")
        : RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

  class MainSettings : public std::enable_shared_from_this<MainSettings> {
   public:
    MainSettings(const std::string &name);

    void AddChild(const std::string &child_name, const MainSettingsChild &child);
    void RemoveChild(const std::string &child_name);
    MainSettingsChild *GetChild(const std::string &child_name);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<MainSettings> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();
    void ModulesRender();

    void RefreshConfig();

    void SaveCurrentConfig();
    std::unordered_map<std::string, MainSettingsChild> m_Childs;

    std::string m_NewProjectPath;
    std::string m_NewModulesPath;
    std::string m_NewPluginsPath;
    std::string m_NewTemplatesPath;
    std::string m_NewContentsPath;
    std::string m_NewVortexVersionPath;

    std::vector<std::string> m_ProjectPoolsPaths;
    std::vector<std::string> m_ModulesPoolsPaths;
    std::vector<std::string> m_PluginsPoolsPaths;
    std::vector<std::string> m_TemplatesPoolsPaths;
    std::vector<std::string> m_ContentsPoolsPaths;
    std::vector<std::string> m_VortexVersionPoolsPaths;

    std::function<void()> m_CreateProjectCallback;
    std::function<void()> m_OpenProjectCallback;
    std::function<void()> m_SettingsCallback;
    std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

    std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
        const std::vector<std::shared_ptr<EnvProject>> &projects,
        size_t maxCount);
    std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
    std::string m_SelectedChildName;

    std::vector<std::string> vortexDists;
    std::string vortexLauncherDist;
    std::string newDist;

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    int selected;
    float leftPaneWidth = 290.0f;
  };
}  // namespace VortexLauncher

#endif  // MAIN_SETTINGS_WINDOW_H