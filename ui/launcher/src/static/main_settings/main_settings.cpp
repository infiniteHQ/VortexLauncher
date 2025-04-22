#include "./main_settings.hpp"

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
  void MainSettings::ModulesRender() {
    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleOne("QSf");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }

  MainSettings::MainSettings(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));
    m_AppWindow->SetClosable(false);

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

    this->AddChild("Help", MainSettingsChild([this]() { }, Cherry::GetPath("resources/imgs/help.png")));
    this->AddChild("Accessibility", MainSettingsChild([this]() { }, Cherry::GetPath("resources/imgs/accessibility.png")));
    this->AddChild("Theme", MainSettingsChild([this]() { }, Cherry::GetPath("resources/imgs/paint.png")));
    this->AddChild("System paths", MainSettingsChild([this]() { }, Cherry::GetPath("resources/imgs/link.png")));
    this->AddChild("Updates & Distribution", MainSettingsChild([this]() { }, Cherry::GetPath("resources/imgs/update.png")));

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::vector<std::shared_ptr<EnvProject>> MainSettings::GetMostRecentProjects(
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

  void MainSettings::AddChild(const std::string &child_name, const MainSettingsChild &child) {
    m_Childs[child_name] = child;
  }

  void MainSettings::RemoveChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &MainSettings::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<MainSettings> MainSettings::Create(const std::string &name) {
    auto instance = std::shared_ptr<MainSettings>(new MainSettings(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void MainSettings::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  MainSettingsChild *MainSettings::GetChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void MainSettings::Render() {
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;

    std::string label = "left_pane" + m_AppWindow->m_Name;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, NULL);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
    ImGui::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/settings2.png")), ImVec2(280, 142));

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

      if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath)
              ->GetData("isClicked") == "true") {
        m_SelectedChildName = child.first;
      }

      // if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(4);

    ImGui::SameLine();
    ImGui::BeginGroup();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);

    if (!m_SelectedChildName.empty()) {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

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

      ImGui::PopStyleVar(2);
    }

    ImGui::EndGroup();
  }
}  // namespace VortexLauncher
