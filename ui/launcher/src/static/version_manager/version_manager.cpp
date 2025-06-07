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

  VersionManager::VersionManager(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_versions.png"));

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
                      ->GetData("isClicked") == "true") {
                VortexMaker::OpenURL("https://vortex.infinite.si/learn");
              }
            },
            Cherry::GetPath("resources/imgs/help.png")));

    this->AddChild(
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
                      "Import a version", Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"))
                      ->GetData("isClicked") == "true") {
                m_WipNotification = true;
                // TODO Open local importation modal
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              if (VortexMaker::GetCurrentContext()->IO.offline) {
                CherryKit::TitleFour("Online mode");  // TODO : LOGO
              } else {
                static std::vector<std::function<void(int)>> available_versions;

                if (available_versions.empty()) {
                  int i = 0;
                  for (auto available_version : VortexMaker::GetCurrentContext()->IO.available_vortex_versions) {
                    i++;
                    available_versions.push_back([available_version, i](int c) {
                      switch (c) {
                        case 0: {
                          CherryKit::ImageHttp(available_version->banner, 140.0f, 50.0f);
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
                                    ->GetData("isClicked") == "true") {
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
                                    ->GetData("isClicked") == "true") {
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
                                  ->GetData("isClicked") == "true") {
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
            Cherry::GetPath("resources/imgs/add.png")));

    this->AddChild(
        "Installed versions",
        VersionManagerChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("Installed Vortex versions");
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
              if (CherryKit::ButtonImageText("Add new version", Cherry::GetPath("resources/base/add.png"))
                      ->GetData("isClicked") == "true") {
                m_SelectedChildName = "Install new version";
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              static std::vector<std::function<void(int)>> versions_render_callbacks;

              if (versions_render_callbacks.empty()) {
                int i = 0;
                for (auto version : VortexMaker::GetCurrentContext()->IO.sys_vortex_versions) {
                  i++;
                  versions_render_callbacks.push_back([version, i](int c) {
                    switch (c) {
                      case 0: {  // Image
                        CherryKit::ImageLocal(Cherry::GetPath(version->banner), 100.0f, 35.0f);
                        break;
                      }
                      case 1: {  // Name
                        CherryStyle::AddMarginY(9.0f);
                        CherryKit::TitleSix(version->name);
                        break;
                      }
                      case 2: {  // Actions
                        CherryStyle::AddMarginY(4.0f);
                        if (CherryKit::ButtonImageText(
                                CherryID("versions_render_callbacks" + std::to_string(i)),
                                "Delete",
                                Cherry::GetPath("resources/imgs/trash.png"))
                                ->GetData("isClicked") == "true") {
                          std::thread([version]() { VortexMaker::OpenVortexUninstaller(version->path); }).detach();
                        }
                        CherryGUI::SameLine();
                        if (CherryKit::ButtonImageText(
                                CherryID("versions_render_callbacks_search" + std::to_string(i)),
                                "",
                                Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
                                ->GetData("isClicked") == "true") {
                          VortexMaker::OpenFolderInFileManager(version->path);
                        }

                        break;
                      }
                      defaut: { break; }
                    }
                  });
                }
              }

              Cherry::SetNextComponentProperty("columns_number", "3");
              Cherry::SetNextComponentProperty("columns_name_0", "");
              Cherry::SetNextComponentProperty("columns_name_1", "Version name");
              Cherry::SetNextComponentProperty("columns_name_2", "Actions");
              Cherry::SetNextComponentProperty("columns_width_0", "3.0f");
              Cherry::SetNextComponentProperty("columns_width_1", "10.0f");
              Cherry::SetNextComponentProperty("columns_width_2", "10.0f");
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

    std::string label = "left_pane_" + m_AppWindow->m_Name;
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true);

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
    CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/versions.png")), ImVec2(280, 142));

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
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);

      if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath)
              ->GetData("isClicked") == "true") {
        m_SelectedChildName = child.first;
      }

      // if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(4);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

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

      CherryGUI::PopStyleVar(2);
    }

    CherryGUI::EndGroup();
  }

}  // namespace VortexLauncher
