#include "./version_manager.hpp"

#include <cstdlib>  // std::system
#include <cstring>
#include <iostream>
#include <string>

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
  void VersionManager::ModulesRender() {
    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleOne("QSf");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }

  void VersionManager::RenderMenubar() {
    CherryGUI::BeginDisabled();
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");

    if (CherryKit::ButtonImageText(
            Cherry::GetLocale("loc.import"), Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"))
            .GetDataAs<bool>("isClicked")) {
      m_SelectedChildName = "Install new version";
    }

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryKit::ButtonImageText("Filters", Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"));

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryKit::ButtonImageText("Search", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));

    CherryGUI::EndDisabled();
  }

  VersionManager::VersionManager(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/vortex_logo.png"));

    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

    m_AppWindow->SetLeftMenubarCallback([this]() { RenderMenubar(); });

    m_AppWindow->m_TabMenuCallback = []() {
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    RefreshVortexVersions();
    m_SelectedChildName = "Installed versions";

    this->AddChild(
        "Help",
        VersionManagerChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#BCBCBC");
              CherryKit::TitleFive("Understanding the Vortex approach.");
              Cherry::PopFont();

              CherryNextProp("color", "#252525");
              CherryKit::Separator();
              CherryNextProp("color_text", "#999999");
              CherryKit::TextWrapped(
                  "The Vortex Launcher lets you manage the versions of the Editor installed on your system—it allows you to "
                  "install, import, update, or remove them. The Vortex Editor is used to launch, run, and edit a tool or "
                  "project. We separate versions and allow multiple ones to coexist so that all tools and projects can work "
                  "together, regardless of their version or compatibility. The launcher acts as a central hub for managing "
                  "and installing different versions of the Vortex Editor.");

              if (CherryKit::ButtonImageTextImage(
                      "Learn and Documentation",
                      Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
                      Cherry::GetPath("resources/imgs/weblink.png"))
                      .GetData("isClicked") == "true") {
                VortexMaker::OpenURL("https://vortex.infinite.si/learn");
              }
            },
            Cherry::GetPath("resources/imgs/help.png")));

    /*this->AddChild(
        "Install new version",
        VersionManagerChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("Install new Vortex Editor versions");
              Cherry::PopFont();
              CherryGUI::SameLine();
              CherryKit::TooltipTextCustom("(?)", []() {
                CherryKit::TitleFour("This is all your Editor versions");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
              });

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");

              if (CherryKit::ButtonImageText(
                      Cherry::GetLocale("loc.import_version"), Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"))
                      .GetData("isClicked") == "true") {
                m_WipNotification = true;
                // TODO Open local importation modal
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              if (VortexMaker::GetCurrentContext()->disconnected) {
                CherryKit::TitleFour(Cherry::GetLocale("loc.offline_mode"));  // TODO : LOGO
              } else {
                static std::vector<std::function<void(int)>> available_versions;

                if (available_versions.empty()) {
                  int i = 0;
                  for (auto available_version : VortexMaker::GetCurrentContext()->IO.available_vortex_versions) {
                    i++;
                    available_versions.push_back([available_version, i](int c) {
                      switch (c) {
                        case 0: {
                          CherryKit::ImageHttp(
                              CherryID(available_version->banner), available_version->banner, 140.0f, 50.0f);
                          break;
                        }
                        case 1: {
                          CherryStyle::AddMarginY(13.0f);
                          CherryKit::TitleFour(available_version->name);
                          CherryGUI::SameLine();
                          Cherry::SetNextComponentProperty("color_text", "#656565");
                          CherryKit::TitleSix("(" + available_version->plat + "/" + available_version->arch + ")");
                          break;
                        }
                        case 2: {  // dist
                          CherryStyle::AddMarginY(13.0f);
                          CherryKit::TitleSix(available_version->dist);
                          break;
                        }
                        case 3: {
                          bool exist = VortexMaker::CheckIfVortexVersionUtilityExist(
                              available_version->version, available_version->path);
                          if (exist) {
                            CherryStyle::AddMarginY(7.0f);
                            if (CherryKit::ButtonImageText(
                                    CherryID("available_versions_reinstall" + std::to_string(i)),
                                    "Reinstall",
                                    Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
                                    .GetData("isClicked") == "true") {
                              std::thread([available_version]() {
                                VortexMaker::OpenVortexUninstaller(available_version->path);
                                VortexMaker::OpenVortexInstaller(
                                    available_version->version,
                                    available_version->arch,
                                    available_version->dist,
                                    available_version->plat);

                                VortexMaker::RefreshEnvironmentVortexVersion();
                              }).detach();
                            }
                          } else {
                            CherryStyle::AddMarginY(7.0f);
                            if (CherryKit::ButtonImageText(
                                    CherryID("available_versions" + std::to_string(i)),
                                    "Install",
                                    Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
                                    .GetData("isClicked") == "true") {
                              std::thread([available_version]() {
                                VortexMaker::OpenVortexInstaller(
                                    available_version->version,
                                    available_version->arch,
                                    available_version->dist,
                                    available_version->plat);
                                VortexMaker::RefreshEnvironmentVortexVersion();
                              }).detach();
                            }
                          }

                          break;
                        }
                        default: {
                          if (CherryKit::ButtonImageText(
                                  CherryID("available_versions_install" + std::to_string(i)),
                                  "Install",
                                  Cherry::GetPath("resources/base/add.png"))
                                  .GetData("isClicked") == "true") {
                            VortexMaker::OpenVortexInstaller(
                                available_version->version,
                                available_version->arch,
                                available_version->dist,
                                available_version->plat);
                          }
                          break;
                        }
                      }
                    });
                  }
                }

                Cherry::SetNextComponentProperty("columns_number", "4");
                Cherry::SetNextComponentProperty("columns_name_0", "");
                Cherry::SetNextComponentProperty("columns_name_1", "Version name");
                Cherry::SetNextComponentProperty("columns_name_2", "Dist");
                Cherry::SetNextComponentProperty("columns_name_3", "Actions");
                Cherry::SetNextComponentProperty("columns_width_0", "5.0f");
                Cherry::SetNextComponentProperty("columns_width_1", "10.0f");
                Cherry::SetNextComponentProperty("columns_width_2", "5.0f");
                Cherry::SetNextComponentProperty("columns_width_3", "10.0f");
                CherryKit::TableCustom(CherryID("sdfg"), "Available Vortex Versions", available_versions);
              }
            },
            Cherry::GetPath("resources/imgs/add.png")));*/

    this->AddChild(
        "Installed versions",
        VersionManagerChild(
            [this]() {
              static std::vector<std::function<void(int)>> versions_render_callbacks;

              if (versions_render_callbacks.empty()) {
                int i = 0;
                for (auto version : m_VortexVersions) {
                  i++;

                  versions_render_callbacks.push_back([version, i](int c) {
                    switch (c) {
                      case 0: {  // Image
                        if (version.system_version) {
                          CherryKit::ImageLocal(Cherry::GetPath(version.system_version->banner), 100.0f, 35.0f);
                        } else {
                          if (version.online_version) {
                            CherryKit::ImageLocal(Cherry::GetHttpPath(version.online_version->banner), 100.0f, 35.0f);
                          }
                        }
                        break;
                      }
                      case 1: {  // Name
                        if (version.system_version) {
                          CherryStyle::AddMarginY(9.0f);
                          CherryKit::TitleSix(version.system_version->proper_name + " " + version.system_version->name);
                        } else {
                          if (version.online_version) {
                            CherryStyle::AddMarginY(9.0f);
                            CherryKit::TitleSix(version.online_version->proper_name + " " + version.online_version->name);
                          }
                        }
                        break;
                      }
                      case 2: {  // Version
                        if (version.system_version) {
                          CherryStyle::AddMarginY(9.0f);
                          CherryKit::TextSimple(version.system_version->version);
                        } else {
                          if (version.online_version) {
                            CherryStyle::AddMarginY(9.0f);
                            CherryKit::TextSimple(version.online_version->version);
                          }
                        }
                        break;
                      }
                      case 3: {  // Platform
                        if (version.system_version) {
                          CherryStyle::AddMarginY(9.0f);
                          CherryKit::TextSimple(version.system_version->plat);
                        } else {
                          if (version.online_version) {
                            CherryStyle::AddMarginY(9.0f);
                            CherryKit::TextSimple(version.online_version->plat);
                          }
                        }
                        break;
                      }
                      case 4: {  // Dist
                        if (version.system_version) {
                          CherryStyle::AddMarginY(9.0f);
                          CherryKit::TextSimple(version.system_version->dist);
                        } else {
                          if (version.online_version) {
                            CherryStyle::AddMarginY(9.0f);
                            CherryKit::TextSimple(version.online_version->dist);
                          }
                        }
                        break;
                      }
                      case 5: {  // Actions
                        if (version.system_version) {
                          CherryStyle::AddMarginY(4.0f);
                          if (CherryKit::ButtonImageText(
                                  CherryID("versions_render_callbacks" + std::to_string(i)),
                                  Cherry::GetLocale("loc.delete"),
                                  Cherry::GetPath("resources/imgs/trash.png"))
                                  .GetData("isClicked") == "true") {
                            std::thread([version]() {
                              VortexMaker::OpenVortexUninstaller(version.system_version->path);
                            }).detach();
                          }
                          CherryGUI::SameLine();
                          if (CherryKit::ButtonImageText(
                                  CherryID("versions_render_callbacks_search" + std::to_string(i)),
                                  "",
                                  Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
                                  .GetData("isClicked") == "true") {
                            VortexMaker::OpenFolderInFileManager(version.system_version->path);
                          }
                        }

                        if (version.online_version) {
                          if (version.online_version_finded) {
                            CherryStyle::AddMarginY(7.0f);
                            CherryGUI::SameLine();
                            if (CherryKit::ButtonImageText(
                                    CherryID("available_versions_reinstall" + std::to_string(i)),
                                    Cherry::GetLocale("loc.reinstall"),
                                    Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
                                    .GetData("isClicked") == "true") {
                              std::thread([version]() {
                                VortexMaker::OpenVortexUninstaller(version.online_version->path);
                                VortexMaker::OpenVortexInstaller(
                                    version.online_version->name,
                                    version.online_version->arch,
                                    version.online_version->dist,
                                    version.online_version->plat);

                                VortexMaker::RefreshEnvironmentVortexVersion();
                              }).detach();
                            }
                          } else {
                            CherryStyle::AddMarginY(4.0f);
                            if (CherryKit::ButtonImageText(
                                    CherryID("available_versions_install" + std::to_string(i)),
                                    Cherry::GetLocale("loc.install"),
                                    Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
                                    .GetData("isClicked") == "true") {
                              std::thread([version]() {
                                VortexMaker::OpenVortexInstaller(
                                    version.online_version->name,
                                    version.online_version->arch,
                                    version.online_version->dist,
                                    version.online_version->plat);
                                VortexMaker::RefreshEnvironmentVortexVersion();
                              }).detach();
                            }
                          }
                        }

                        break;
                      }
                      defaut: { break; }
                    }
                  });
                }
              }

              Cherry::SetNextComponentProperty("columns_number", "6");
              Cherry::SetNextComponentProperty("columns_name_0", "");
              Cherry::SetNextComponentProperty("columns_name_1", "Name");
              Cherry::SetNextComponentProperty("columns_name_2", "Version");
              Cherry::SetNextComponentProperty("columns_name_3", "Platform");
              Cherry::SetNextComponentProperty("columns_name_4", "Dist");
              Cherry::SetNextComponentProperty("columns_name_5", "Actions");
              Cherry::SetNextComponentProperty("columns_width_0", "1.0f");
              Cherry::SetNextComponentProperty("columns_width_1", "3.0f");
              Cherry::SetNextComponentProperty("columns_width_5", "2.0f");
              CherryKit::TableCustom(CherryID("qsf"), "Installed Vortex versions", versions_render_callbacks);
            },
            Cherry::GetPath("resources/imgs/stack.png")));

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::vector<std::shared_ptr<EnvProject>> VersionManager::GetMostRecentProjects(
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

  void VersionManager::AddChild(const std::string &child_name, const VersionManagerChild &child) {
    m_Childs[child_name] = child;
  }

  void VersionManager::RemoveChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &VersionManager::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<VersionManager> VersionManager::Create(const std::string &name) {
    auto instance = std::shared_ptr<VersionManager>(new VersionManager(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void VersionManager::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  VersionManagerChild *VersionManager::GetChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void VersionManager::Render() {
    CherryKit::NotificationButton(
        &m_WipNotification,
        4,
        "info",
        "Work in progress",
        "This feature is not available yet. Thanks for your patience.",
        []() { });

    if (!m_SelectedChildName.empty()) {
      CherryStyle::RemoveMarginY(9.0f);
      if (CherryGUI::BeginChild(
              "ChildPanelVManager",
              ImVec2(0, 0),
              false,
              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        auto child = GetChild(m_SelectedChildName);

        if (child) {
          std::function<void()> pannel_render = child->RenderCallback;
          if (pannel_render) {
            pannel_render();
          }
        }
      }
      CherryGUI::EndChild();
    }
  }

}  // namespace VortexLauncher
