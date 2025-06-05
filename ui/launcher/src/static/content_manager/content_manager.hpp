#pragma once

#include "../../../../../main/include/contents/delete.h"
#include "../../../../../main/include/contents/install.h"
#include "../../../../../main/include/contents/load.h"
#include "../../../../../main/include/templates/delete.h"
#include "../../../../../main/include/templates/install.h"
#include "../../../../../main/include/templates/load.h"
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../instances/asset_finder/asset_finder.hpp"

#ifndef CONTENT_WINDOW_H
#define CONTENT_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher {
  // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

  struct ContentManagerChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    ContentManagerChild(
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined")
        : RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

  class ContentManager : public std::enable_shared_from_this<ContentManager> {
   public:
    ContentManager(const std::string &name);

    void AddChild(const std::string &child_name, const ContentManagerChild &child);
    void RemoveChild(const std::string &child_name);
    ContentManagerChild *GetChild(const std::string &child_name);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<ContentManager> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();
    void ModulesRender();

    bool ItemContentCard(
        const std::string &name,
        const std::string &path,
        const std::string &description,
        const std::string &size,
        bool selected,
        const std::string &logo,
        ImU32 bgColor = IM_COL32(100, 100, 100, 255),
        ImU32 borderColor = IM_COL32(150, 150, 150, 255),
        ImU32 lineColor = IM_COL32(255, 255, 0, 255),
        float maxTextWidth = 100.0f,
        float borderRadius = 5.0f,
        const std::shared_ptr<ContentInterface> &content = nullptr);

    std::unordered_map<std::string, ContentManagerChild> m_Childs;

    std::shared_ptr<AssetFinder> m_AssetFinder;

    bool m_WipNotification = false;

    std::vector<std::shared_ptr<TemplateInterface>> m_FindedTemplates;
    std::vector<std::shared_ptr<TemplateInterface>> m_SelectedTemplates;
    std::vector<std::shared_ptr<TemplateInterface>> m_TemplatesToSuppr;
    std::vector<std::shared_ptr<TemplateInterface>> m_TemplatesToImport;

    std::vector<std::shared_ptr<ContentInterface>> m_FindedContents;
    std::vector<std::shared_ptr<ContentInterface>> m_SelectedContents;
    std::vector<std::shared_ptr<ContentInterface>> m_ContentsToSuppr;
    std::vector<std::shared_ptr<ContentInterface>> m_ContentsToImport;

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

#endif  // CONTENT_WINDOW_H