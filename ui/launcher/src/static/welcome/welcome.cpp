#include "./welcome.hpp"

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

void OpenURL(const std::string &url) {
#if defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
  std::string command = "open " + url;
  system(command.c_str());
#elif defined(__linux__)
  std::string command = "xdg-open " + url;
  system(command.c_str());
#else
  std::cerr << "Unsupported platform: unable to open URL." << std::endl;
#endif
}

bool EndsWith(const std::string &value, const std::string &suffix) {
  if (suffix.size() > value.size())
    return false;
  return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

namespace VortexLauncher {
  void WelcomeWindow::WelcomeRender() {
    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleOne(Cherry::GetLocale("loc.windows.welcome.overview"));
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();

    CherryKit::Space(5.0f);

    CherryNextProp("color_text", "#797979");
    CherryKit::TitleSix("Fast actions");

    static std::vector<std::shared_ptr<Cherry::Component>> actions_blocks;

    if (actions_blocks.empty()) {
      actions_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::IdentifierProperty::CreateOnly,
          m_CreateProjectCallback,
          269.0f,
          120.0f,
          {
              []() { CherryKit::Space(20.0f); },
              []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/add.png"), 30, 30); },
              []() { CherryKit::Space(15.0f); },
              []() { CherryKit::TextCenter("Create a project"); },
          }));

      actions_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::IdentifierProperty::CreateOnly,
          m_OpenProjectCallback,
          269.0f,
          120.0f,
          {
              []() { CherryKit::Space(20.0f); },
              []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/open.png"), 30, 30); },
              []() { CherryKit::Space(15.0f); },
              []() { CherryKit::TextCenter("Open a project"); },
          }));

      actions_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::IdentifierProperty::CreateOnly,
          m_SettingsCallback,
          269.0f,
          120.0f,
          {
              []() { CherryKit::Space(20.0f); },
              []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/settings.png"), 30, 30); },
              []() { CherryKit::Space(15.0f); },
              []() { CherryKit::TextCenter("Settings & Configurations"); },
          }));
    }

    // Draw grid with blocks
    CherryKit::GridSimple(270.0f, 270.0f, actions_blocks);

    CherryKit::Space(5.0f);

    CherryNextProp("color_text", "#797979");
    CherryKit::TitleSix("Latest projects & tools");

    static std::vector<std::shared_ptr<Cherry::Component>> blocks;

    if (blocks.empty()) {
      auto recentProjects = VortexMaker::GetRecentProjects(4);

      int i = 0;
      for (auto project : recentProjects) {
        if (project) {
          blocks.push_back(CherryKit::BlockVerticalCustom(
              Cherry::IdentifierProperty::CreateOnly,
              [=]() { m_ProjectCallback(project); },
              200.0f,
              120.0f,
              {
                  [=]() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 200, 75); },
                  [=]() {
                    CherryStyle::AddMarginX(5.0f);
                    CherryKit::TitleSix(project->name);
                  },
                  [=]() {
                    CherryStyle::AddMarginX(5.0f);
                    CherryStyle::RemoveYMargin(5.0f);
                    CherryStyle::PushFontSize(0.70f);
                    CherryKit::TextSimple(project->lastOpened);
                    CherryStyle::PopFontSize();
                  },
              },
              i));
        }

        i++;
      }

      while (i < 4) {
        std::string str1;
        std::string str2;
        if (i == 0) {
          str1 = "No project yet";
          str2 = "Create one :)";
        } else {
          str1 = "No more project";
          str2 = "Create another :)";
        }
        blocks.push_back(CherryKit::BlockVerticalCustom(
            Cherry::IdentifierProperty::CreateOnly,
            m_CreateProjectCallback,
            200.0f,
            120.0f,
            {
                [=]() {
                  CherryKit::Space(40.0f);
                  CherryStyle::PushFontSize(0.8f);
                  CherryNextProp("color_text", "#787878");
                  CherryKit::TextCenter(str1);
                  CherryStyle::PopFontSize();
                  CherryNextProp("color_text", "#787878");
                  CherryStyle::PushFontSize(0.6f);
                  CherryKit::TextCenter(str2);
                  CherryStyle::PopFontSize();
                },
            },
            i));
        i++;
      }
    }
    CherryKit::GridSimple(200.0f, 200.0f, blocks);

    CherryKit::Space(20.0f);
    CherryNextProp("color", "#222222");

    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleThree("Latest news");
    Cherry::PopFont();
    CherryKit::Separator();

    static std::vector<std::shared_ptr<Cherry::Component>> news_blocks;

    if (VortexMaker::GetCurrentContext()->IO.offline) {
      if (news_blocks.empty()) {
        auto block = CherryKit::BannerImageContext(
            Cherry::IdentifierProperty::CreateOnly,
            402.0f,
            140.0f,
            Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"),
            "",
            "");
        news_blocks.push_back(block);
        news_blocks.push_back(block);
      }
    } else {
      if (news_blocks.empty()) {
        for (const auto &article : VortexMaker::GetCurrentContext()->IO.news) {
          if (!article.image_link.empty() &&
              (EndsWith(article.image_link, ".png") || EndsWith(article.image_link, ".jpg")) &&
              (article.image_link.find("http://") == 0 || article.image_link.find("https://") == 0)) {
            auto block = CherryKit::BannerImageContext(
                Cherry::IdentifierProperty::CreateOnly,
                402.0f,
                140.0f,
                Cherry::GetHttpPath(article.image_link),
                article.title,
                article.description);
            news_blocks.push_back(block);
          }
        }
      }
    }

    CherryKit::GridSimple(400.0f, 400.0f, news_blocks);

    CherryKit::Space(20.0f);
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleThree("Latest Vortex versions");
    Cherry::PopFont();
    CherryKit::Separator();

    static std::vector<std::shared_ptr<Cherry::Component>> last_versions_blocks;

    if (last_versions_blocks.empty()) {
      int version_index = 0;
      for (auto version : VortexMaker::GetCurrentContext()->latest_vortex_versions) {
        if (version_index < 3) {
          auto block = CherryKit::BlockVerticalCustom(
              Cherry::IdentifierProperty::CreateOnly,
              [=]() {},
              200.0f,
              120.0f,
              {
                  [=]() { CherryKit::ImageHttp(version.banner, 260, 75); },
                  [=]() {
                    CherryStyle::AddMarginX(5.0f);
                    CherryKit::TitleSix(version.name);
                  },
                  /*CherryLambda
                  (
                      CherryStyle::AddMarginX(5.0f);
                      CherryStyle::RemoveYMargin(5.0f);
                      CherryStyle::PushFontSize(0.70f);
                      CherryKit::TextSimple(version.already_installed);
                      CherryStyle::PopFontSize();
                  ),*/
              });
          last_versions_blocks.push_back(block);

          version_index++;
        }
      }
    }

    CherryKit::GridSimple(150.0f, 150.0f, last_versions_blocks);
  }

  WelcomeWindow::WelcomeWindow(const std::string &name) {
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

    m_SelectedChildName = "?loc:loc.windows.welcome.overview";
    m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);

    this->AddChild(
        "Support Us", WelcomeWindowChild([this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/heart.png")));
    this->AddChild(
        "What's news", WelcomeWindowChild([this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/web.png")));
    this->AddChild("Forums", WelcomeWindowChild([this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/forum.png")));
    this->AddChild(
        "Documentation", WelcomeWindowChild([this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/docs.png")));

    this->AddChild(
        "?loc:loc.windows.welcome.overview",
        WelcomeWindowChild(
            [this]() { this->WelcomeRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::vector<std::shared_ptr<EnvProject>> WelcomeWindow::GetMostRecentProjects(
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

  void WelcomeWindow::AddChild(const std::string &child_name, const WelcomeWindowChild &child) {
    m_Childs[child_name] = child;
  }

  void WelcomeWindow::RemoveChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &WelcomeWindow::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<WelcomeWindow> WelcomeWindow::Create(const std::string &name) {
    auto instance = std::shared_ptr<WelcomeWindow>(new WelcomeWindow(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void WelcomeWindow::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  WelcomeWindowChild *WelcomeWindow::GetChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void WelcomeWindow::Render() {
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
    ImGui::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner.png")), ImVec2(280, 142));

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
      CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath);
      if (false) {
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
