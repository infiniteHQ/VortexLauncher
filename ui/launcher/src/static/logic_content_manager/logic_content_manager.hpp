#pragma once

#include "../../../../../main/include/modules/delete.h"
#include "../../../../../main/include/modules/install.h"
#include "../../../../../main/include/modules/load.h"
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../instances/asset_finder/asset_finder.hpp"

#ifndef LOGICAL_CONTENT_WINDOW_H
#define LOGICAL_CONTENT_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher {
  // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

  struct LogicalContentManagerChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    LogicalContentManagerChild(
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined")
        : RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

  class LogicalContentManager : public std::enable_shared_from_this<LogicalContentManager> {
   public:
    LogicalContentManager(const std::string &name);

    void AddChild(const std::string &child_name, const LogicalContentManagerChild &child);
    void RemoveChild(const std::string &child_name);
    LogicalContentManagerChild *GetChild(const std::string &child_name);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<LogicalContentManager> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();
    void ModulesRender();
    void SearchModulesOnDirectory(const std::string &path);

    std::unordered_map<std::string, LogicalContentManagerChild> m_Childs;

    std::shared_ptr<AssetFinder> m_AssetFinder;

    std::vector<std::shared_ptr<ModuleInterface>> m_FindedModules;
    std::vector<std::shared_ptr<ModuleInterface>> m_SelectedModules;
    std::vector<std::shared_ptr<ModuleInterface>> m_ModulesToSuppr;
    std::vector<std::shared_ptr<ModuleInterface>> m_ModulesToImport;

    std::atomic<bool> m_StillSearching = false;
    bool m_SearchStarted = false;
    std::string m_SearchElapsedTime;

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

#endif  // LOGICAL_CONTENT_WINDOW_H