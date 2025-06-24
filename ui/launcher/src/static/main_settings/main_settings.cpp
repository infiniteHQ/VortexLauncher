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

  static void saveVortexVersions(const std::vector<std::string> &paths, const std::string &jsonFilePath) {
    nlohmann::json jsonData;
    jsonData["vortex_versions_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file) {
      file << jsonData.dump(4);
    }
  }

  static void loadVortexVersions(std::vector<std::string> &paths, const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (file) {
      nlohmann::json jsonData;
      file >> jsonData;
      for (const auto &path : jsonData["vortex_versions_pools"]) {
        paths.push_back(path.get<std::string>());
      }
    }
  }

  static void saveTemplates(const std::vector<std::string> &paths, const std::string &jsonFilePath) {
    nlohmann::json jsonData;
    jsonData["templates_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file) {
      file << jsonData.dump(4);
    }
  }

  static void loadTemplates(std::vector<std::string> &paths, const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (file) {
      nlohmann::json jsonData;
      file >> jsonData;
      for (const auto &path : jsonData["templates_pools"]) {
        paths.push_back(path.get<std::string>());
      }
    }
  }

  static void saveContents(const std::vector<std::string> &paths, const std::string &jsonFilePath) {
    nlohmann::json jsonData;
    jsonData["contents_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file) {
      file << jsonData.dump(4);
    }
  }

  static void loadContents(std::vector<std::string> &paths, const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (file) {
      nlohmann::json jsonData;
      file >> jsonData;
      for (const auto &path : jsonData["contents_pools"]) {
        paths.push_back(path.get<std::string>());
      }
    }
  }
  static void saveModules(const std::vector<std::string> &paths, const std::string &jsonFilePath) {
    nlohmann::json jsonData;
    jsonData["modules_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file) {
      file << jsonData.dump(4);
    }
  }

  static void loadModules(std::vector<std::string> &paths, const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (file) {
      nlohmann::json jsonData;
      file >> jsonData;
      for (const auto &path : jsonData["modules_pools"]) {
        paths.push_back(path.get<std::string>());
      }
    }
  }

  static void savePlugins(const std::vector<std::string> &paths, const std::string &jsonFilePath) {
    nlohmann::json jsonData;
    jsonData["plugins_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file) {
      file << jsonData.dump(4);
    }
  }

  static void loadPlugins(std::vector<std::string> &paths, const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (file) {
      nlohmann::json jsonData;
      file >> jsonData;
      for (const auto &path : jsonData["plugins_pools"]) {
        paths.push_back(path.get<std::string>());
      }
    }
  }

  static void saveProjects(const std::vector<std::string> &projectPaths, const std::string &jsonFilePath) {
    nlohmann::json jsonData;
    jsonData["projects_pools"] = projectPaths;

    std::ofstream file(jsonFilePath);
    if (file) {
      file << jsonData.dump(4);
    }
  }

  static void loadProjects(std::vector<std::string> &projectPaths, const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (file) {
      nlohmann::json jsonData;
      file >> jsonData;
      for (const auto &path : jsonData["projects_pools"]) {
        projectPaths.push_back(path.get<std::string>());
      }
    }
  }

  void PathListEditor(const std::string &type, std::vector<std::string> *list, std::string *newPath) {
    if (!list || !newPath)
      return;

    CherryGUI::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
    if (CherryGUI::BeginTable("##project_paths", 2)) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

      CherryGUI::TableSetupColumn("Path");
      CherryGUI::TableSetupColumn("Action");
      CherryGUI::TableHeadersRow();

      for (size_t i = 0; i < list->size(); ++i) {
        CherryGUI::TableNextRow();

        CherryGUI::TableSetColumnIndex(0);
        CherryGUI::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

        CherryGUI::Text("%s", (*list)[i].c_str());

        CherryGUI::PopStyleColor();

        CherryGUI::TableSetColumnIndex(1);
        if (CherryKit::ButtonImageText(
                CherryID("delete_entry" + type + std::to_string(i)), "", Cherry::GetPath("resources/imgs/trash.png"))
                ->GetData("isClicked") == "true") {
          list->erase(list->begin() + i);
          --i;
        }
      }

      CherryGUI::PopStyleVar();
      CherryGUI::EndTable();
    }

    CherryGUI::PopStyleColor();

    if (CherryKit::ButtonImageText(
            CherryID("add_entry" + type), "", Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
            ->GetData("isClicked") == "true") {
      if (!newPath->empty()) {
        list->push_back(*newPath);
        newPath->clear();
      }
    }

    CherryGUI::SameLine();

    CherryKit::InputString("", newPath);
  }

  void MainSettings::ModulesRender() {
    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleOne("QSf");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }

  std::string GetConfigFilePath() {
    std::string homeDir = VortexMaker::getHomeDirectory();
    std::string configPath;
    if (VortexMaker::IsWindows()) {
      configPath = homeDir + "\\.vx\\configs\\dists.json";
    } else {
      configPath = homeDir + "/.vx/configs/dists.json";
    }
    return configPath;
  }

  nlohmann::json LoadConfig(const std::string &filePath) {
    std::ifstream file(filePath);
    nlohmann::json config;
    if (file.is_open()) {
      file >> config;
    } else {
      config = { { "vortex_dists", { "def" } }, { "vortexlauncher_dist", "def" } };
    }
    return config;
  }

  void SaveConfig(const std::string &filePath, const nlohmann::json &config) {
    std::ofstream file(filePath);
    if (file.is_open()) {
      file << config.dump(4);
    }
  }

  void MainSettings::RefreshConfig() {
    auto config = LoadConfig(GetConfigFilePath());
    vortexDists = config["vortex_dists"].get<std::vector<std::string>>();
    vortexLauncherDist = config["vortexlauncher_dist"].get<std::string>();
    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

#ifdef _WIN32
    path = VortexMaker::convertPathToWindowsStyle(path);
#endif

    m_ProjectPoolsPaths.clear();
    m_ModulesPoolsPaths.clear();
    m_PluginsPoolsPaths.clear();
    m_TemplatesPoolsPaths.clear();
    m_ContentsPoolsPaths.clear();
    m_VortexVersionPoolsPaths.clear();
    loadProjects(m_ProjectPoolsPaths, path + "projects_pools.json");
    loadModules(m_ModulesPoolsPaths, path + "modules_pools.json");
    loadPlugins(m_PluginsPoolsPaths, path + "plugins_pools.json");
    loadTemplates(m_TemplatesPoolsPaths, path + "templates_pools.json");
    loadContents(m_ContentsPoolsPaths, path + "contents_pools.json");
    loadVortexVersions(m_VortexVersionPoolsPaths, path + "vortex_versions_pools.json");
  }

  void MainSettings::SaveCurrentConfig() {
    nlohmann::json config;
    config["vortex_dists"] = vortexDists;
    config["vortexlauncher_dist"] = vortexLauncherDist;
    SaveConfig(GetConfigFilePath(), config);

    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

#ifdef _WIN32
    path = VortexMaker::convertPathToWindowsStyle(path);
#endif

    saveProjects(m_ProjectPoolsPaths, path + "projects_pools.json");
    saveModules(m_ModulesPoolsPaths, path + "modules_pools.json");
    savePlugins(m_PluginsPoolsPaths, path + "plugins_pools.json");
    saveTemplates(m_TemplatesPoolsPaths, path + "templates_pools.json");
    saveContents(m_ContentsPoolsPaths, path + "contents_pools.json");
    saveVortexVersions(m_VortexVersionPoolsPaths, path + "vortex_versions_pools.json");
  }

  MainSettings::MainSettings(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

    RefreshConfig();

    m_AppWindow->m_TabMenuCallback = []() {
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    m_SelectedChildName = "Updates & Distribution";
    m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);

    this->AddChild(
        "Help",
        MainSettingsChild(
            [this]() {
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
        "Accessibility",
        MainSettingsChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("Accessibility settings");
              Cherry::PopFont();
              CherryNextProp("color", "#252525");
              CherryKit::Separator();
              CherryKit::TextSimple("No settings for the moment. Work in progress");
            },
            Cherry::GetPath("resources/imgs/accessibility.png")));
    this->AddChild(
        "Theme",
        MainSettingsChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("Theme settings");
              Cherry::PopFont();
              CherryNextProp("color", "#252525");
              CherryKit::Separator();
              CherryKit::TextSimple("No settings for the moment. Work in progress");
            },
            Cherry::GetPath("resources/imgs/paint.png")));
    this->AddChild(
        "System paths",
        MainSettingsChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("System paths settings");
              Cherry::PopFont();
              CherryGUI::SameLine();
              CherryKit::TooltipTextCustom("(?)", []() {
                CherryKit::TitleFour("em : Editor Modules");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
                CherryStyle::AddMarginY(10.0f);
                CherryKit::TitleFour("esm : Editor Script Modules");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
              });

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_refresh.png"))
                      ->GetData("isClicked") == "true") {
                RefreshConfig();
              }

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_refresh.png"))
                      ->GetData("isClicked") == "true") {
                SaveCurrentConfig();
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              Cherry::SetNextComponentProperty("header_text_visible", "false");
              Cherry::SetNextComponentProperty("cell_padding_y_row", "2.0");

              CherryKit::TableSimple(
                  "Settings",
                  {
                      CherryKit::KeyValCustom(
                          "Project paths",
                          [=]() { PathListEditor("Project paths", &m_ProjectPoolsPaths, &m_NewProjectPath); }),
                      CherryKit::KeyValCustom(
                          "Vortex version paths",
                          [=]() {
                            PathListEditor("Vortex version paths", &m_VortexVersionPoolsPaths, &m_NewVortexVersionPath);
                          }),
                      CherryKit::KeyValCustom(
                          "Modules paths",
                          [=]() { PathListEditor("Modules paths", &m_ModulesPoolsPaths, &m_NewModulesPath); }),
                      CherryKit::KeyValCustom(
                          "Plugins paths",
                          [=]() { PathListEditor("Plugins paths", &m_PluginsPoolsPaths, &m_NewPluginsPath); }),
                      CherryKit::KeyValCustom(
                          "Templates paths",
                          [=]() { PathListEditor("Templates paths", &m_TemplatesPoolsPaths, &m_NewTemplatesPath); }),
                      CherryKit::KeyValCustom(
                          "Contents paths",
                          [=]() { PathListEditor("Contents paths", &m_ContentsPoolsPaths, &m_NewContentsPath); }),
                  });
            },
            Cherry::GetPath("resources/imgs/link.png")));
    this->AddChild(
        "Updates & Distribution",
        MainSettingsChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("Updates & Distribution settings");
              Cherry::PopFont();
              CherryGUI::SameLine();
              CherryKit::TooltipTextCustom("(?)", []() {
                CherryKit::TitleFour("em : Editor Modules");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
                CherryStyle::AddMarginY(10.0f);
                CherryKit::TitleFour("esm : Editor Script Modules");
                CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
              });

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_refresh.png"))
                      ->GetData("isClicked") == "true") {
                RefreshConfig();
              }

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_refresh.png"))
                      ->GetData("isClicked") == "true") {
                SaveCurrentConfig();
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              Cherry::SetNextComponentProperty("header_text_visible", "false");
              Cherry::SetNextComponentProperty("cell_padding_y_row", "2.0");

              CherryKit::TableSimple(
                  "Settings",
                  {
                      CherryKit::KeyValCustom(
                          "Vortex distributions",
                          [=]() {
                            if (CherryGUI::BeginTable("##vortex_dists", 2)) {
                              CherryGUI::TableSetupColumn("Path");
                              CherryGUI::TableSetupColumn("Action");
                              CherryGUI::TableHeadersRow();
                              std::cout << vortexDists.size() << std::endl;
                              for (size_t i = 0; i < vortexDists.size(); ++i) {
                                CherryGUI::TableNextRow();
                                CherryGUI::TableSetColumnIndex(0);
                                CherryGUI::Text("%s", vortexDists[i].c_str());

                                CherryGUI::TableSetColumnIndex(1);

                                if (CherryKit::ButtonImageText(
                                        CherryID("delete" + std::to_string(i)),
                                        "",
                                        Cherry::GetPath("resources/imgs/trash.png"))
                                        ->GetData("isClicked") == "true") {
                                  vortexDists.erase(vortexDists.begin() + i);
                                  --i;
                                }
                              }
                              CherryGUI::EndTable();
                            }

                            CherryKit::InputString("Add", &newDist);
                            CherryGUI::SameLine();
                            if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/base/add.png"))
                                    ->GetData("isClicked") == "true") {
                              vortexDists.push_back(newDist);
                            }
                          }),
                      CherryKit::KeyValString("Vortex Launcher distribution", &vortexLauncherDist),
                  });
            },
            Cherry::GetPath("resources/imgs/update.png")));

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
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, NULL);

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
    CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/settings2.png")), ImVec2(280, 142));

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
    CherryGUI::PopStyleVar(4);
    CherryGUI::PopStyleColor(2);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

      if (CherryGUI::BeginChild(
              "ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
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
