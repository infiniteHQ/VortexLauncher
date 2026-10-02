#pragma once

#include "../../../../../main/include/modules/delete.h"
#include "../../../../../main/include/modules/install.h"
#include "../../../../../main/include/modules/load.h"
#include "../../../../../main/include/plugins/delete.h"
#include "../../../../../main/include/plugins/install.h"
#include "../../../../../main/include/plugins/load.h"
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../instances/asset_finder/asset_finder.hpp"

#ifndef LOGICAL_CONTENT_WINDOW_H
#define LOGICAL_CONTENT_WINDOW_H
#include "../../../../../lib/cherry/cherry.hpp"
#include "./content_flash_window.hpp"

namespace VortexLauncher {

  struct ContentEntry {
    std::string name, proper_name, version, description;
    std::string logo, banner, path;
    std::vector<std::string> supported_versions;
  };

  struct ContentKind {
    std::string id;
    std::string label;
    std::string icon;
    std::string finder_icon;
    bool banner_card = false;
    std::function<std::vector<std::string>&()> pools;
    std::function<std::vector<ContentEntry>()> list;
    std::function<bool(const std::string&, ContentEntry&)> probe;
    std::function<void(const std::string&, const std::string&)> install;
    std::function<void(const std::string&, const std::string&)> remove;
    std::function<void()> reload;
    bool (*check)(const std::string&) = nullptr;
  };

  struct InstallSource {
    std::string path;
    std::string cleanup;
  };

  struct PendingInstall {
    std::string path;
    std::string cleanup;
    ContentEntry entry;
    std::vector<ContentEntry> existing;
    bool identical = false;
    int decision = 0;
  };

  struct StagedItem {
    std::string kind_id;
    std::string dir;
    std::string pool;
  };

  struct KindState {
    std::vector<ContentEntry> entries;
    std::vector<PendingInstall> pending;
    std::string pending_pool;
    ContentEntry to_delete;
    char search[128] = "";
    bool delete_request = false;
    bool conflict_request = false;
  };

  struct LogicalContentManagerChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    LogicalContentManagerChild(
        const std::function<void()>& rendercallback = []() { },
        const std::string& logopath = "undefined")
        : RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

  class LogicalContentManager : public std::enable_shared_from_this<LogicalContentManager> {
   public:
    LogicalContentManager(const std::string& name);

    void AddChild(const std::string& child_name, const LogicalContentManagerChild& child);
    void RemoveChild(const std::string& child_name);
    LogicalContentManagerChild* GetChild(const std::string& child_name);

    std::shared_ptr<Cherry::AppWindow>& GetAppWindow();
    static std::shared_ptr<LogicalContentManager> Create(const std::string& name);
    void SetupRenderCallback();
    void Render();
    void ModulesRender();
    void SearchModulesOnDirectory(const std::string& path);

    void BuildKinds();
    void Rebuild(const ContentKind& kind);
    void OpenImport(const ContentKind& kind);
    void SpawnFlashWindow(const std::string& mode);
    void HandleFinderResult(const ContentKind& kind);
    void HandleStaged();
    void BeginInstall(const ContentKind& kind, const std::vector<InstallSource>& sources, const std::string& pool);
    void ApplyInstalls(const ContentKind& kind);
    void CancelInstalls(const ContentKind& kind);
    void RenderKind(const ContentKind& kind);
    void RenderConflictModal(const ContentKind& kind);
    void RenderDeleteModal(const ContentKind& kind);
    void DrawCard(const ContentKind& kind, const ContentEntry& e, float w, float h);
    void DrawRow(const ContentKind& kind, const ContentEntry& e, float w);

    bool m_ListView = false;
    std::vector<ContentKind> m_Kinds;
    std::map<std::string, KindState> m_States;
    std::vector<StagedItem> m_StagedQueue;
    std::string m_ImportKind;
    int m_FlashCounter = 0;

    std::unordered_map<std::string, LogicalContentManagerChild> m_Childs;

    std::shared_ptr<AssetFinder> m_AssetFinder;

    std::vector<std::shared_ptr<ModuleInterface>> m_FindedModules;
    std::vector<std::shared_ptr<ModuleInterface>> m_SelectedModules;
    std::vector<std::shared_ptr<ModuleInterface>> m_ModulesToSuppr;
    std::vector<std::shared_ptr<ModuleInterface>> m_ModulesToImport;

    std::vector<std::shared_ptr<PluginInterface>> m_FindedPlugins;
    std::vector<std::shared_ptr<PluginInterface>> m_SelectedPlugins;
    std::vector<std::shared_ptr<PluginInterface>> m_PluginsToSuppr;
    std::vector<std::shared_ptr<PluginInterface>> m_PluginsToImport;

    std::atomic<bool> m_StillSearching = false;
    bool m_SearchStarted = false;
    std::string m_SearchElapsedTime;

    bool m_WipNotification = false;

    std::function<void()> m_CreateProjectCallback;
    std::function<void()> m_OpenProjectCallback;
    std::function<void()> m_SettingsCallback;
    std::function<void(const std::shared_ptr<EnvProject>&)> m_ProjectCallback;

    std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
        const std::vector<std::shared_ptr<EnvProject>>& projects,
        size_t maxCount);
    std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
    std::string m_SelectedChildName;

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    int selected;
    float leftPaneWidth = 290.0f;
  };
}  // namespace VortexLauncher

#endif  // LOGICAL_CONTENT_WINDOW_H