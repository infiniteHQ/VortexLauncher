#include "./logic_content_manager.hpp"

#include <cstdlib>  // std::system
#include <cstring>
#include <iostream>
#include <string>

#include "../../../../../main/include/modules/delete.h"  // TODO : include directory to cmake
#include "../../../../../main/include/plugins/delete.h"  // TODO : include directory to cmake

#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

namespace VortexLauncher {
  void LogicalContentManager::ModulesRender() {
    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleOne("QSf");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }

  void LogicalContentManager::SearchModulesOnDirectory(const std::string &path) {
    m_StillSearching = true;
    m_SearchStarted = true;
    VortexMaker::FindModulesInDirectoryRecursively(path, m_FindedModules, m_StillSearching, m_SearchElapsedTime);
  }

  LogicalContentManager::LogicalContentManager(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png"));

    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

    m_AppWindow->m_TabMenuCallback = []() {
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    m_SelectedChildName = "Plugins";
    m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);

    this->AddChild(
        "Help",
        LogicalContentManagerChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#BCBCBC");
              CherryKit::TitleFive("Understanding the Vortex approach.");
              Cherry::PopFont();

              CherryNextProp("color", "#252525");
              CherryKit::Separator();
              CherryNextProp("color_text", "#999999");
              CherryKit::TextWrapped(
                  "Logical content consists of modular components that add features to a project and the Vortex Editor. The "
                  "Vortex Editor alone simply provides the project context and enables all logical and static content to "
                  "communicate with each other. The editor guarantees interoperability between components. It allows all "
                  "parts to interact and provides essential utilities like content managers, project settings, and the "
                  "content browser—along with logging and debugging tools.");

              if (CherryKit::ButtonImageTextImage(
                      "Learn and Documentation",
                      Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
                      Cherry::GetPath("resources/imgs/weblink.png"))
                      ->GetData("isClicked") == "true") {
                VortexMaker::OpenURL("https://vortex.infinite.si/learn");
              }
            },
            Cherry::GetPath("resources/imgs/help.png")));
    this->AddChild(
        "Plugins",
        LogicalContentManagerChild(
            [this]() {
              static std::vector<std::shared_ptr<Cherry::Component>> plugins_block;
              static bool delete_plugin_modal_opened = false;
              static bool import_plugin_modal_opened = false;
              static std::shared_ptr<PluginInterface> plugin_to_delete = nullptr;

              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("All plugins in the system");
              Cherry::PopFont();
              ImGui::SameLine();
              CherryKit::TooltipTextCustom("(?)", []() {
                CherryKit::TitleFour("em : Editor plugins");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
                CherryStyle::AddMarginY(10.0f);
                CherryKit::TitleFour("esm : Editor Script plugins");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
              });

              ImGui::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") ==
                  "true") {
                m_AssetFinder = AssetFinder::Create("Import plugins(s)", VortexMaker::getHomeDirectory());
                Cherry::ApplicationSpecification spec;

                std::string name = "Find plugins(s)";
                spec.Name = name;
                spec.MinHeight = 500;
                spec.MinWidth = 500;
                spec.Height = 600;
                spec.Width = 1150;
                spec.DisableTitle = false;
                spec.CustomTitlebar = true;
                spec.DisableWindowManagerTitleBar = true;
                spec.WindowOnlyClosable = false;
                spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
                spec.UniqueAppWindowName = m_AssetFinder->GetAppWindow()->m_Name;
                spec.UsingCloseCallback = true;
                spec.FavIconPath = Cherry::GetPath("resources/imgs/vplug.png");
                spec.IconPath = Cherry::GetPath("resources/imgs/vplug.png");
                spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
                spec.WindowSaves = false;
                spec.MenubarCallback = [this]() {
                  if (ImGui::BeginMenu("Window")) {
                    if (ImGui::MenuItem("Close")) {
                      Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
                    }
                    ImGui::EndMenu();
                  }
                };

                m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);

                if (!VortexMaker::GetCurrentContext()->IO.sys_plugins_pools.empty()) {
                  m_AssetFinder->m_TargetPossibilities = VortexMaker::GetCurrentContext()->IO.sys_plugins_pools;
                }

                m_AssetFinder->GetAppWindow()->SetVisibility(true);
                m_AssetFinder->m_ItemToReconize.push_back(std::make_shared<AssetFinderItem>(
                    VortexMaker::CheckPluginInDirectory, "Plugin sample", "Plugin", Cherry::HexToRGBA("#B1FF31")));

                Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
              }

              ImGui::SameLine();
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");

              if (CherryKit::ButtonImageText("Browse", Cherry::GetPath("resources/imgs/icons/misc/icon_net.png"))
                      ->GetData("isClicked") == "true") {
                m_WipNotification = true;
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              if (m_AssetFinder) {
                if (m_AssetFinder->m_GetFileBrowserPath) {
                  m_AssetFinder->m_GetFileBrowserPath = false;
                  m_FindedPlugins.clear();

                  // SearchModulesOnDirectory(ContentPath);
                  std::string pool;
                  if (!VortexMaker::GetCurrentContext()->IO.sys_plugins_pools[m_AssetFinder->m_TargetPoolIndex].empty()) {
                    pool = VortexMaker::GetCurrentContext()->IO.sys_plugins_pools[m_AssetFinder->m_TargetPoolIndex];
                  }

                  for (auto selected : m_AssetFinder->m_Selected) {
                    VortexMaker::InstallPluginToSystem(selected, pool);
                  }

                  plugins_block.clear();
                  VortexMaker::LoadSystemPlugins(VortexMaker::GetCurrentContext()->IO.sys_ep);

                  m_AssetFinder->GetAppWindow()->SetVisibility(false);
                  m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
                }
              }

              CherryKit::ModalSimple("Import plugin(s)", &import_plugin_modal_opened, [this]() {

              });

              CherryKit::ModalSimple("Delete a plugin", &delete_plugin_modal_opened, []() {
                if (!plugin_to_delete) {
                  delete_plugin_modal_opened = true;
                  return;
                }

                CherryKit::TextWrapped(
                    "Important: This action will not delete selected plugins from projects you've got, this action will "
                    "only "
                    "uninstall project available from your system. To delete plugins of your projects, go on the project "
                    "editor, "
                    "plugins manager and delete plugins manually.");
                CherryKit::Separator();

                if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                        ->GetData("isClicked") == "true") {
                  delete_plugin_modal_opened = false;
                }
                CherryGUI::SameLine();

                if (CherryKit::ButtonImageText("Confirm Delete", Cherry::GetPath("resources/imgs/trash.png"))
                        ->GetData("isClicked") == "true") {
                  VortexMaker::DeleteSystemPlugin(plugin_to_delete->m_name, plugin_to_delete->m_version);
                  plugins_block.clear();
                  VortexMaker::LoadSystemPlugins(VortexMaker::GetCurrentContext()->IO.sys_ep);
                  delete_plugin_modal_opened = false;
                }
              });

              if (plugins_block.empty()) {
                for (auto sysplugin : VortexMaker::GetCurrentContext()->IO.sys_ep) {
                  plugins_block.push_back(CherryKit::BlockVerticalCustom(
                      Cherry::IdentifierPattern(Cherry::IdentifierProperty::CreateOnly, sysplugin->m_name),
                      m_CreateProjectCallback,
                      200.0f,
                      130.0f,
                      {
                          [sysplugin]() {
                            CherryStyle::AddMarginX(5.0f);
                            CherryStyle::AddMarginY(5.0f);
                            CherryKit::ImageLocal(Cherry::GetPath(sysplugin->m_logo_path), 28.0f, 28.0f);

                            CherryGUI::SameLine();
                            Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
                            CherryStyle::AddMarginY(7.0f);
                            CherryKit::TextSimple(sysplugin->m_proper_name);
                          },
                          [sysplugin]() {
                            CherryStyle::AddMarginX(5.0f);
                            CherryStyle::RemoveYMargin(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#686868");
                            CherryKit::TextSimple(sysplugin->m_version);

                            CherryGUI::SameLine();

                            CherryKit::TooltipTextCustom("(?)", [sysplugin]() {
                              CherryKit::SeparatorText("Vortex compat");
                              CherryKit::TextWrapped(
                                  std::to_string(sysplugin->m_supported_versions.size()) + " supported version(s)");
                              for (auto v : sysplugin->m_supported_versions) {
                                Cherry::SetNextComponentProperty("color_text", "#888888");
                                CherryKit::TextSimple(v);
                              }

                              // TODO versions...
                              CherryKit::SeparatorText("Platform(s)");
                              // TODO : enum supported platforms
                            });

                            CherryStyle::AddMarginX(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#9B9B9B");
                            std::string wrapped_description = sysplugin->m_description;
                            if (wrapped_description.size() > 75) {
                              wrapped_description = wrapped_description.substr(0, 72);
                              wrapped_description.append("...");
                            }
                            CherryKit::TextWrapped(wrapped_description);
                          },
                          [sysplugin]() {
                            Cherry::SetNextComponentProperty("color", "#353535");
                            CherryKit::Separator();

                            CherryStyle::RemoveYMargin(20.0f);
                            CherryGUI::BeginChild("###action_bar", ImVec2(200, 50), false, ImGuiWindowFlags_NoScrollbar);
                            CherryStyle::AddMarginX(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#888888");

                            CherryGUI::SameLine();
                            if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/trash.png"))
                                    ->GetData("isClicked") == "true") {
                              std::cout << "True" << std::endl;

                              plugin_to_delete = sysplugin;
                              delete_plugin_modal_opened = true;
                            }

                            CherryGUI::SameLine();

                            if (CherryKit::ButtonImageText(
                                    "", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
                                    ->GetData("isClicked") == "true") {
                              VortexMaker::OpenFolderInFileManager(sysplugin->m_path);
                            }

                            CherryGUI::EndChild();
                          },
                      }));
                }
              }

              // Draw grid with blocks
              Cherry::PushPermanentProperty("block_border_radius", "6.0");
              CherryKit::GridSimple(270.0f, 270.0f, &plugins_block);
              Cherry::PopPermanentProperty();
            },
            Cherry::GetPath("resources/imgs/plug.png")));
    this->AddChild(
        "Modules",
        LogicalContentManagerChild(
            [this]() {
              static std::vector<std::shared_ptr<Cherry::Component>> modules_block;
              static bool delete_module_modal_opened = false;
              static bool import_module_modal_opened = false;
              static std::shared_ptr<ModuleInterface> module_to_delete = nullptr;

              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("All modules in the system");
              Cherry::PopFont();
              ImGui::SameLine();
              CherryKit::TooltipTextCustom("(?)", []() {
                CherryKit::TitleFour("em : Editor Modules");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
                CherryStyle::AddMarginY(10.0f);
                CherryKit::TitleFour("esm : Editor Script Modules");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
              });

              ImGui::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") ==
                  "true") {
                m_AssetFinder = AssetFinder::Create("Import module(s)", VortexMaker::getHomeDirectory());
                Cherry::ApplicationSpecification spec;

                std::string name = "Find module(s)";
                spec.Name = name;
                spec.MinHeight = 500;
                spec.MinWidth = 500;
                spec.Height = 600;
                spec.Width = 1150;
                spec.DisableTitle = false;
                spec.CustomTitlebar = true;
                spec.DisableWindowManagerTitleBar = true;
                spec.WindowOnlyClosable = false;
                spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
                spec.UniqueAppWindowName = m_AssetFinder->GetAppWindow()->m_Name;
                spec.UsingCloseCallback = true;
                spec.FavIconPath = Cherry::GetPath("resources/imgs/vbox.png");
                spec.IconPath = Cherry::GetPath("resources/imgs/vbox.png");
                spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
                spec.WindowSaves = false;
                spec.MenubarCallback = [this]() {
                  if (ImGui::BeginMenu("Window")) {
                    if (ImGui::MenuItem("Close")) {
                      Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
                    }
                    ImGui::EndMenu();
                  }
                };

                m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);

                if (!VortexMaker::GetCurrentContext()->IO.sys_modules_pools.empty()) {
                  m_AssetFinder->m_TargetPossibilities = VortexMaker::GetCurrentContext()->IO.sys_modules_pools;
                }

                m_AssetFinder->GetAppWindow()->SetVisibility(true);
                m_AssetFinder->m_ItemToReconize.push_back(std::make_shared<AssetFinderItem>(
                    VortexMaker::CheckModuleInDirectory, "Module sample", "Module", Cherry::HexToRGBA("#B1FF31")));

                Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
              }

              ImGui::SameLine();
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Browse", Cherry::GetPath("resources/imgs/icons/misc/icon_net.png"))
                      ->GetData("isClicked") == "true") {
                m_WipNotification = true;
              }
              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              if (m_AssetFinder) {
                if (m_AssetFinder->m_GetFileBrowserPath) {
                  m_AssetFinder->m_GetFileBrowserPath = false;
                  m_FindedModules.clear();

                  // SearchModulesOnDirectory(ContentPath);
                  std::string pool;
                  if (!VortexMaker::GetCurrentContext()->IO.sys_modules_pools[m_AssetFinder->m_TargetPoolIndex].empty()) {
                    pool = VortexMaker::GetCurrentContext()->IO.sys_modules_pools[m_AssetFinder->m_TargetPoolIndex];
                  }

                  for (auto selected : m_AssetFinder->m_Selected) {
                    VortexMaker::InstallModuleToSystem(selected, pool);
                  }

                  modules_block.clear();
                  VortexMaker::LoadSystemModules(VortexMaker::GetCurrentContext()->IO.sys_em);

                  m_AssetFinder->GetAppWindow()->SetVisibility(false);
                  m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
                }
              }

              CherryKit::ModalSimple("Import module(s)", &import_module_modal_opened, [this]() {

              });

              CherryKit::ModalSimple("Delete a module", &delete_module_modal_opened, []() {
                if (!module_to_delete) {
                  delete_module_modal_opened = true;
                  return;
                }

                CherryKit::TextWrapped(
                    "Important: This action will not delete selected modules from projects you've got, this action will "
                    "only "
                    "uninstall project available from your system. To delete modules of your projects, go on the project "
                    "editor, "
                    "modules manager and delete modules manually.");
                CherryKit::Separator();

                if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                        ->GetData("isClicked") == "true") {
                  delete_module_modal_opened = false;
                }
                CherryGUI::SameLine();

                if (CherryKit::ButtonImageText("Confirm Delete", Cherry::GetPath("resources/imgs/trash.png"))
                        ->GetData("isClicked") == "true") {
                  VortexMaker::DeleteSystemModule(module_to_delete->m_name, module_to_delete->m_version);
                  modules_block.clear();
                  VortexMaker::LoadSystemModules(VortexMaker::GetCurrentContext()->IO.sys_em);
                  delete_module_modal_opened = false;
                }
              });

              if (modules_block.empty()) {
                for (auto sysmodule : VortexMaker::GetCurrentContext()->IO.sys_em) {
                  modules_block.push_back(CherryKit::BlockVerticalCustom(
                      Cherry::IdentifierPattern(Cherry::IdentifierProperty::CreateOnly, sysmodule->m_name),
                      m_CreateProjectCallback,
                      269.0f,
                      160.0f,
                      {
                          [sysmodule]() {
                            if (sysmodule->m_banner_path.empty()) {
                              CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 269.0f);
                            }

                            else {
                              CherryKit::ImageLocal(sysmodule->m_banner_path, 269.0f);
                            }
                          },
                          [sysmodule]() {
                            CherryStyle::RemoveYMargin(35.0f);
                            CherryStyle::AddMarginX(10.0f);
                            CherryKit::ImageLocal(Cherry::GetPath(sysmodule->m_logo_path), 40.0f, 40.0f);
                          },
                          [sysmodule]() {
                            CherryStyle::AddMarginX(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
                            CherryKit::TextSimple(sysmodule->m_proper_name);
                            CherryGUI::SameLine();
                            Cherry::SetNextComponentProperty("color_text", "#686868");
                            CherryKit::TextSimple(sysmodule->m_version);

                            CherryGUI::SameLine();

                            CherryKit::TooltipTextCustom("(?)", [sysmodule]() {
                              CherryKit::SeparatorText("Vortex compat");
                              CherryKit::TextWrapped(
                                  std::to_string(sysmodule->m_supported_versions.size()) + " supported version(s)");
                              for (auto v : sysmodule->m_supported_versions) {
                                Cherry::SetNextComponentProperty("color_text", "#888888");
                                CherryKit::TextSimple(v);
                              }

                              // TODO versions...
                              CherryKit::SeparatorText("Platform(s)");
                              // TODO : enum supported platforms
                            });

                            CherryStyle::AddMarginX(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#9B9B9B");
                            std::string wrapped_description = sysmodule->m_description;
                            if (wrapped_description.size() > 75) {
                              wrapped_description = wrapped_description.substr(0, 72);
                              wrapped_description.append("...");
                            }
                            CherryKit::TextWrapped(wrapped_description);
                          },
                          [sysmodule]() {
                            Cherry::SetNextComponentProperty("color", "#353535");
                            CherryKit::Separator();

                            CherryStyle::RemoveYMargin(20.0f);
                            CherryGUI::BeginChild("###action_bar", ImVec2(200, 50), false, ImGuiWindowFlags_NoScrollbar);
                            CherryStyle::AddMarginX(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#888888");

                            CherryGUI::SameLine();
                            if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/trash.png"))
                                    ->GetData("isClicked") == "true") {
                              std::cout << "True" << std::endl;

                              module_to_delete = sysmodule;
                              delete_module_modal_opened = true;
                            }

                            CherryGUI::SameLine();

                            if (CherryKit::ButtonImageText(
                                    "", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
                                    ->GetData("isClicked") == "true") {
                              VortexMaker::OpenFolderInFileManager(sysmodule->m_path);
                            }

                            CherryGUI::EndChild();
                          },
                      }));
                }
              }

              // Draw grid with blocks
              Cherry::PushPermanentProperty("block_border_radius", "6.0");
              CherryKit::GridSimple(270.0f, 270.0f, &modules_block);
              Cherry::PopPermanentProperty();
            },
            Cherry::GetPath("resources/imgs/box.png")));

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::vector<std::shared_ptr<EnvProject>> LogicalContentManager::GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>> &projects,
      size_t maxCount) {
    auto sortedProjects = projects;
    std::sort(
        sortedProjects.begin(),
        sortedProjects.end(),
        [](const std::shared_ptr<EnvProject> &a, const std::shared_ptr<EnvProject> &b) {
          return a->lastOpened > b->lastOpened;
        });

    if (sortedProjects.size() > maxCount) {
      sortedProjects.resize(maxCount);
    }
    return sortedProjects;
  }

  void LogicalContentManager::AddChild(const std::string &child_name, const LogicalContentManagerChild &child) {
    m_Childs[child_name] = child;
  }

  void LogicalContentManager::RemoveChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &LogicalContentManager::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<LogicalContentManager> LogicalContentManager::Create(const std::string &name) {
    auto instance = std::shared_ptr<LogicalContentManager>(new LogicalContentManager(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void LogicalContentManager::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  LogicalContentManagerChild *LogicalContentManager::GetChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void LogicalContentManager::Render() {
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;

    CherryKit::NotificationButton(
        &m_WipNotification,
        4,
        "info",
        "Work in progress",
        "This feature is not available yet. Thanks for your patience.",
        []() { });

    std::string label = "left_pane" + m_AppWindow->m_Name;
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
    ImGui::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/lc_banner.png")), ImVec2(280, 142));

    // CherryStyle::SetPadding(7.0f);

    for (const auto &child : m_Childs) {
      if (child.first == m_SelectedChildName) {
        // opt.hex_text_idle = "#FFFFFFFF";
      } else {
        // opt.hex_text_idle = "#A9A9A9FF";
      }
      std::string child_name;

      if (child.first.rfind("?loc:", 0) == 0) {
        std::string localeName = child.first.substr(5);
        child_name = Cherry::GetLocale(localeName) + "####" + localeName;
      } else {
        child_name = child.first;
      }

      CherryNextProp("color_bg", "#00000000");
      CherryNextProp("color_border", "#00000000");
      CherryNextProp("padding_x", "2");
      CherryNextProp("padding_y", "2");
      CherryNextProp("size_x", "20");
      CherryNextProp("size_y", "20");
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7.5f);

      // SetProperty("padding_x", "10");

      if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath)
              ->GetData("isClicked") == "true") {
        m_SelectedChildName = child.first;
      }

      if (child_name == "Modules")  // TODO (WARN) : Prepare to locales
      {
        CherryGUI::SameLine();
        Cherry::SetNextComponentProperty("color_text", "#676767");
        CherryStyle::AddMarginY(3.5f);
        CherryKit::TextSimple("(" + std::to_string(VortexMaker::GetCurrentContext()->IO.sys_em.size()) + ")");
        CherryStyle::RemoveYMargin(3.5f);
      } else if (child_name == "Plugins")  // TODO (WARN) : Prepare to locales
      {
        CherryGUI::SameLine();
        Cherry::SetNextComponentProperty("color_text", "#676767");
        CherryStyle::AddMarginY(3.5f);
        CherryKit::TextSimple("(" + std::to_string(VortexMaker::GetCurrentContext()->IO.sys_ep.size()) + ")");
        CherryStyle::RemoveYMargin(3.5f);
      }

      // if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
    }

    ImGui::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(4);

    ImGui::SameLine();
    ImGui::BeginGroup();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

      if (ImGui::BeginChild(
              "ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        auto child = GetChild(m_SelectedChildName);

        if (child) {
          std::function<void()> pannel_render = child->RenderCallback;
          if (pannel_render) {
            pannel_render();
          }
        }
      }
      ImGui::EndChild();

      CherryGUI::PopStyleVar(2);
    }

    ImGui::EndGroup();
  }
}  // namespace VortexLauncher
