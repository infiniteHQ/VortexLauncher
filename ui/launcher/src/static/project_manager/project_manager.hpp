#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../instances/asset_finder/asset_finder.hpp"
#include "../../utils.hpp"

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

class ProjectManager : public std::enable_shared_from_this<ProjectManager> {
 public:
  ProjectManager(const std::string &name);

  void addModuleModal();

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ProjectManager> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();

  void Refresh();
  void Update();

  /**
   * @brief Menu items
   */
  void mainButtonsMenuItem();
  void filterMenuItem();
  void createMenuItem();
  void searchMenuItem();

  VxContext *ctx;
  std::string parent;

  bool m_ProjectCreation = false;
  bool m_ProjectOpening = false;
  bool m_Settings = false;

  std::shared_ptr<EnvProject> m_SelectedEnvproject;
  std::shared_ptr<EnvProject> m_SelectedEnvprojectToRemove;

 private:
  bool opened;

  std::shared_ptr<VxToolchain> latest_toolchain;

  std::thread receiveThread;
  std::vector<std::shared_ptr<TemplateInterface>> project_templates;
  std::shared_ptr<Cherry::Image> m_ListIcon;
  std::shared_ptr<Cherry::Image> m_RefreshIcon;
  std::shared_ptr<Cherry::Image> m_OpenIcon;
  std::shared_ptr<Cherry::Image> m_ProjectIcon;
  std::shared_ptr<Cherry::Image> m_AddIcon;
  std::shared_ptr<Cherry::Image> m_TrashIcon;

  std::shared_ptr<AssetFinder> m_AssetFinder;

  // Create project values
  std::string v_ProjectName = "New Project";
  std::string v_ProjectVersion = "";
  std::string v_ProjectAuthor = "";
  std::string v_ProjectDescription = "";
  std::vector<std::string> projectPoolsPaths;
  bool v_ProjectOpen;
  std::atomic<bool> m_StillSearching = false;
  bool SearchStarted = false;
  std::string m_ElapsedTime;

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;

  bool CollapseAll = false;
};

#endif  // PROJECTMANAGER_H