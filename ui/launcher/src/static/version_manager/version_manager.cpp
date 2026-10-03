#include "./version_manager.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

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
  namespace {

    enum class VFilter { All, Installed, Available };

    char g_Search[128] = "";
    VFilter g_Filter = VFilter::All;
    int g_ConfirmDelete = -1;

    constexpr float kCardHeight = 64.0f;

    struct Theme {
      bool dark;
      const char* card;
      const char* cardBorder;
      const char* text;
      const char* subText;
      const char* separator;
      const char* pillBg;
      const char* pillText;
      const char* accent;
      const char* accentText;
      const char* danger;
    };

    Theme GetTheme() {
      ImVec4 bg = CherryGUI::GetStyleColorVec4(ImGuiCol_WindowBg);
      float lum = 0.299f * bg.x + 0.587f * bg.y + 0.114f * bg.z;
      if (lum < 0.5f) {
        return { true,      "#1E1E1E", "#2C2C2C", "#E6E6E6", "#8C8C8C", "#2A2A2A",
                 "#2B2B2B", "#BDBDBD", "#B1FF31", "#101010", "#FF5C5C" };
      }
      return { false,     "#F3F3F3", "#D8D8D8", "#1B1B1B", "#6B6B6B", "#DADADA",
               "#E4E4E4", "#444444", "#3E8E00", "#FFFFFF", "#D32F2F" };
    }

    ImVec4 Hex(const char* hex) {
      unsigned long v = std::strtoul(hex + 1, nullptr, 16);
      return ImVec4(((v >> 16) & 0xFF) / 255.0f, ((v >> 8) & 0xFF) / 255.0f, (v & 0xFF) / 255.0f, 1.0f);
    }

    float PillWidth(const std::string& t) {
      return CherryGUI::CalcTextSize(t.c_str()).x + 16.0f;
    }

    float PillHeight() {
      return CherryGUI::GetTextLineHeight() + 6.0f;
    }

    void Pill(const std::string& t, const ImVec4& bg, const ImVec4& fg) {
      ImVec2 ts = CherryGUI::CalcTextSize(t.c_str());
      ImVec2 size(ts.x + 16.0f, ts.y + 6.0f);
      ImVec2 p = CherryGUI::GetCursorScreenPos();
      ImDrawList* dl = CherryGUI::GetWindowDrawList();
      dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), CherryGUI::ColorConvertFloat4ToU32(bg), size.y * 0.5f);
      dl->AddText(ImVec2(p.x + 8.0f, p.y + 3.0f), CherryGUI::ColorConvertFloat4ToU32(fg), t.c_str());
      CherryGUI::Dummy(size);
    }

    struct Row {
      std::string name, version, plat, dist, banner;
      bool installed = false;
      bool hasOnline = false;
      std::string sysPath;
      std::string onName, onArch, onDist, onPlat, onPath;
    };

    template<class V>
    Row BuildRow(const V& v) {
      Row r;
      if (v.system_version) {
        r.installed = true;
        r.name = v.system_version->proper_name;
        r.version = v.system_version->version;
        r.plat = v.system_version->plat;
        r.dist = v.system_version->dist;
        r.banner = Cherry::GetPath(v.system_version->banner);
        r.sysPath = v.system_version->path;
      } else if (v.online_version) {
        r.name = v.online_version->proper_name;
        r.version = v.online_version->version;
        r.plat = v.online_version->plat;
        r.dist = v.online_version->dist;
        r.banner = Cherry::GetHttpPath(v.online_version->banner);
        if (v.online_version_finded)
          r.installed = true;
      }
      if (v.online_version) {
        r.hasOnline = true;
        r.onName = v.online_version->name;
        r.onArch = v.online_version->arch;
        r.onDist = v.online_version->dist;
        r.onPlat = v.online_version->plat;
        r.onPath = v.online_version->path;
      }
      return r;
    }

    std::string Lower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
      return s;
    }

    bool MatchesSearch(const Row& r) {
      if (g_Search[0] == '\0')
        return true;
      std::string q = Lower(g_Search);
      return Lower(r.name).find(q) != std::string::npos || Lower(r.version).find(q) != std::string::npos ||
             Lower(r.dist).find(q) != std::string::npos || Lower(r.plat).find(q) != std::string::npos;
    }

    void RenderCard(const Row& r, int idx, const Theme& th) {
      CherryGUI::PushID(idx);
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Hex(th.card));
      CherryGUI::PushStyleColor(ImGuiCol_Border, Hex(th.cardBorder));
      CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

      if (CherryGUI::BeginChild(
              "##vcard", ImVec2(0, kCardHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        const float w = CherryGUI::GetWindowWidth();

        CherryGUI::SetCursorPos(ImVec2(12.0f, (kCardHeight - 35.0f) * 0.5f - 1.0f));
        CherryKit::ImageLocal(r.banner, 100.0f, 35.0f);
        CherryGUI::SameLine(0.0f, 14.0f);

        CherryGUI::BeginGroup();
        CherryGUI::SetCursorPosY(11.0f);
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", th.text);
        CherryKit::TitleSix(r.name);
        Cherry::PopFont();
        CherryNextProp("color_text", th.subText);
        CherryKit::TextSimple(r.plat);
        CherryGUI::EndGroup();

        const bool canReinstall = r.installed && r.hasOnline;
        const float actionsW = r.installed ? (canReinstall ? 300.0f : 150.0f) : 120.0f;
        const float pillsW = PillWidth(r.version) + PillWidth(r.dist) + 6.0f;
        const float startX = w - 14.0f - actionsW - pillsW - 12.0f;

        CherryGUI::SameLine();
        CherryGUI::SetCursorPos(ImVec2(startX, (kCardHeight - PillHeight()) * 0.5f));
        Pill("v" + r.version, Hex(th.accent), Hex(th.accentText));
        CherryGUI::SameLine(0.0f, 6.0f);
        Pill(r.dist, Hex(th.pillBg), Hex(th.pillText));

        CherryGUI::SameLine(0.0f, 12.0f);
        CherryGUI::BeginGroup();
        CherryNextComponent.SetProperty("padding_x", "8.0f");
        CherryNextComponent.SetProperty("padding_y", "5.0f");

        if (!r.installed) {
          CherryGUI::SetCursorPosY(15.0f);
          if (CherryKit::ButtonImageText(
                  CherryID("vm_install"),
                  Cherry::GetLocale("loc.install"),
                  Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
                  .GetData("isClicked") == "true") {
            Row c = r;
            std::thread([c]() {
              VortexMaker::OpenVortexInstaller(c.onName, c.onArch, c.onDist, c.onPlat);
              VortexMaker::RefreshEnvironmentVortexVersion();
            }).detach();
          }
        } else {
          CherryGUI::SetCursorPosY(15.0f);
          if (CherryKit::ButtonImageText(
                  CherryID("vm_folder"), "", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
                  .GetData("isClicked") == "true") {
            VortexMaker::OpenFolderInFileManager(r.sysPath);
          }

          if (canReinstall) {
            CherryGUI::SameLine(0.0f, 8.0f);
            CherryGUI::SetCursorPosY(15.0f);
            if (CherryKit::ButtonImageText(
                    CherryID("vm_reinstall"),
                    Cherry::GetLocale("loc.reinstall"),
                    Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
                    .GetData("isClicked") == "true") {
              Row c = r;
              std::thread([c]() {
                VortexMaker::OpenVortexUninstaller(c.sysPath.empty() ? c.onPath : c.sysPath);
                VortexMaker::OpenVortexInstaller(c.onName, c.onArch, c.onDist, c.onPlat);
                VortexMaker::RefreshEnvironmentVortexVersion();
              }).detach();
            }
          }

          if (!r.sysPath.empty()) {
            CherryGUI::SameLine(0.0f, 8.0f);
            CherryGUI::SetCursorPosY(15.0f);
            const bool confirming = (g_ConfirmDelete == idx);
            if (confirming)
              CherryNextProp("color_text", th.danger);
            if (CherryKit::ButtonImageText(
                    CherryID("vm_delete"),
                    confirming ? "Confirm ?" : Cherry::GetLocale("loc.delete"),
                    Cherry::GetPath("resources/imgs/trash.png"))
                    .GetData("isClicked") == "true") {
              if (confirming) {
                g_ConfirmDelete = -1;
                std::string path = r.sysPath;
                std::thread([path]() {
                  VortexMaker::OpenVortexUninstaller(path);
                  VortexMaker::RefreshEnvironmentVortexVersion();
                }).detach();
              } else {
                g_ConfirmDelete = idx;
              }
            }
          }
        }
        CherryGUI::EndGroup();
      }
      CherryGUI::EndChild();

      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor(2);
      CherryGUI::PopID();
      CherryStyle::AddMarginY(6.0f);
    }

    void RenderSectionHeader(const char* title, size_t count, const Theme& th) {
      CherryStyle::AddMarginY(6.0f);
      Cherry::PushFont("ClashBold");
      CherryNextProp("color_text", th.text);
      CherryKit::TitleFive(title);
      Cherry::PopFont();
      CherryGUI::SameLine();
      CherryNextProp("color_text", th.subText);
      CherryKit::TextSimple("(" + std::to_string(count) + ")");
      CherryNextProp("color", th.separator);
      CherryKit::Separator();
      CherryStyle::AddMarginY(4.0f);
    }

    template<class List>
    void RenderVersionList(const List& versions, bool offline, const Theme& th) {
      std::vector<Row> installed, available;
      for (const auto& v : versions) {
        Row r = BuildRow(v);
        if (!MatchesSearch(r))
          continue;
        if (r.installed) {
          if (g_Filter != VFilter::Available)
            installed.push_back(std::move(r));
        } else if (r.hasOnline) {
          if (g_Filter != VFilter::Installed)
            available.push_back(std::move(r));
        }
      }

      int idx = 0;

      if (g_Filter != VFilter::Available) {
        RenderSectionHeader("Installed", installed.size(), th);
        if (installed.empty()) {
          CherryNextProp("color_text", th.subText);
          CherryKit::TextSimple(g_Search[0] ? "No installed version matches your search." : "No version installed yet.");
        }
        for (const auto& r : installed)
          RenderCard(r, idx++, th);
      }

      if (g_Filter != VFilter::Installed) {
        RenderSectionHeader("Available to download", available.size(), th);
        if (offline) {
          CherryNextProp("color_text", th.danger);
          CherryKit::TextSimple(Cherry::GetLocale("loc.offline_mode"));
        } else if (available.empty()) {
          CherryNextProp("color_text", th.subText);
          CherryKit::TextSimple(g_Search[0] ? "No downloadable version matches your search." : "Everything is up to date.");
        }
        for (const auto& r : available)
          RenderCard(r, idx++, th);
      }
    }

  }  // namespace

  void VersionManager::ModulesRender() {
    const Theme th = GetTheme();
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", th.subText);
    CherryKit::TitleOne("QSf");
    Cherry::PopFont();
    CherryNextProp("color", th.separator);
    CherryKit::Separator();
  }

  void VersionManager::RenderMenubar() {
    const Theme th = GetTheme();

    int nInstalled = 0, nAvailable = 0;
    for (const auto& v : m_VortexVersions) {
      Row r = BuildRow(v);
      if (r.installed)
        nInstalled++;
      else if (r.hasOnline)
        nAvailable++;
    }

    CherryStyle::AddMarginX(10.0f);
    CherryGUI::SameLine(0.0f, 0.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText(
            CherryID("vm_import"),
            Cherry::GetLocale("loc.import"),
            Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"))
            .GetData("isClicked") == "true") {
      m_WipNotification = true;  // TODO
    }

    CherryGUI::SameLine(0.0f, 8.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText(
            CherryID("vm_refresh"), "Refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
            .GetData("isClicked") == "true") {
      RefreshVortexVersions();
    }

    CherryGUI::SameLine(0.0f, 16.0f);
    CherryGUI::SetNextItemWidth(260.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
    CherryGUI::PushStyleColor(ImGuiCol_FrameBg, Hex(th.card));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Hex(th.cardBorder));
    CherryGUI::PushStyleColor(ImGuiCol_Text, Hex(th.text));
    CherryGUI::InputTextWithHint("##vm_search", "Search name, version, dist...", g_Search, sizeof(g_Search));
    CherryGUI::PopStyleColor(3);
    CherryGUI::PopStyleVar(2);

    auto chip = [&](const char* label, VFilter f) {
      const bool active = (g_Filter == f);
      CherryGUI::SameLine(0.0f, 6.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.0f);
      CherryGUI::PushStyleColor(ImGuiCol_Button, active ? Hex(th.accent) : Hex(th.pillBg));
      CherryGUI::PushStyleColor(ImGuiCol_Text, active ? Hex(th.accentText) : Hex(th.pillText));
      if (CherryGUI::Button(label))
        g_Filter = f;
      CherryGUI::PopStyleColor(2);
      CherryGUI::PopStyleVar();
    };
    chip(("All (" + std::to_string(nInstalled + nAvailable) + ")").c_str(), VFilter::All);
    chip(("Installed (" + std::to_string(nInstalled) + ")").c_str(), VFilter::Installed);
    chip(("Available (" + std::to_string(nAvailable) + ")").c_str(), VFilter::Available);

    CherryGUI::NewLine();
    CherryNextProp("color", th.separator);
    CherryKit::Separator();
  }

  VersionManager::VersionManager(const std::string& name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/vortex_logo.png"));

    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

    m_AppWindow->m_TabMenuCallback = []() { };

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    RefreshVortexVersions();
    m_SelectedChildName = "Installed versions";

    this->AddChild(
        "Help",
        VersionManagerChild(
            [this]() {
              const Theme th = GetTheme();
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", th.text);
              CherryKit::TitleFive("Understanding the Vortex approach.");
              Cherry::PopFont();

              CherryNextProp("color", th.separator);
              CherryKit::Separator();
              CherryNextProp("color_text", th.subText);
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

    this->AddChild(
        "Installed versions",
        VersionManagerChild(
            [this]() {
              const Theme th = GetTheme();
              CherryStyle::AddMarginY(8.0f);
              RenderMenubar();

              const bool offline = VortexMaker::GetCurrentContext()->disconnected;
              CherryStyle::AddMarginX(10.0f);
              RenderVersionList(m_VortexVersions, offline, th);
            },
            Cherry::GetPath("resources/imgs/stack.png")));
  }

  std::vector<std::shared_ptr<EnvProject>> VersionManager::GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>>& projects,
      size_t maxCount) {
    auto sortedProjects = projects;
    std::sort(
        sortedProjects.begin(),
        sortedProjects.end(),
        [](const std::shared_ptr<EnvProject>& a, const std::shared_ptr<EnvProject>& b) {
          return a->lastOpened > b->lastOpened;
        });

    if (sortedProjects.size() > maxCount) {
      sortedProjects.resize(maxCount);
    }
    return sortedProjects;
  }

  void VersionManager::AddChild(const std::string& child_name, const VersionManagerChild& child) {
    m_Childs[child_name] = child;
  }

  void VersionManager::RemoveChild(const std::string& child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow>& VersionManager::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<VersionManager> VersionManager::Create(const std::string& name) {
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

  VersionManagerChild* VersionManager::GetChild(const std::string& child_name) {
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

  void VersionManager::OpenWithSearch(const std::string& query) {
    std::strncpy(g_Search, query.c_str(), sizeof(g_Search) - 1);
    g_Search[sizeof(g_Search) - 1] = '\0';
    g_Filter = VFilter::All;
    g_ConfirmDelete = -1;
    m_SelectedChildName = "Installed versions";
    RefreshVortexVersions();
    if (m_AppWindow)
      m_AppWindow->SetVisibility(true);
  }
}  // namespace VortexLauncher