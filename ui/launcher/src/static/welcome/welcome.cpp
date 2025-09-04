#include "./welcome.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>  // std::system
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_set>
static std::vector<bool> selectedRows(finded_projects.size(), false);

std::vector<std::string> modules_projects;
static std::vector<int> selectedIDs;

int projetct_import_dest_index;
std::vector<std::string> project_pools;
std::string projetct_import_dest;

static bool no_installed_modal_opened;
static bool multiple_versions_modal_opened;
static bool already_running_modal_opened;
static bool open_deletion_modal = false;

static std::string no_installed_version;
static std::string no_installed_project_name;
static std::string no_installed_project_picture;
static VortexVersion no_installed_version_available;
static std::vector<std::shared_ptr<VortexVersion>> all_versions_for_project;
#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

bool EndsWith(const std::string &value, const std::string &suffix) {
  if (suffix.size() > value.size())
    return false;
  return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

static std::string test;
namespace VortexLauncher {
  void WelcomeWindow::CreateProjectRender() {
    float columnWidths[3] = { 140.0f, 250.0f, 150.0f };

    ImVec2 windowSize = CherryGUI::GetWindowSize();

    float availableWidth = windowSize.x - CherryGUI::GetStyle().ItemSpacing.x * 2;

    float totalColumnWidths = 0.0f;
    for (int i = 0; i < 3; ++i)
      totalColumnWidths += columnWidths[i];

    float columnProportions[3];
    for (int i = 0; i < 3; ++i)
      columnProportions[i] = columnWidths[i] / totalColumnWidths;

    float total_x = CherryGUI::GetContentRegionAvail().x;

    CherryStyle::AddMarginX(5.0f);
    CherryGUI::BeginChild("###sdgf5mid", ImVec2(total_x - 330, 0), false, ImGuiWindowFlags_NoBackground);
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleFive(Cherry::GetLocale("loc.menubar.welcome.select_a_base_and_create"));
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();

    for (int project = 0; project < project_templates.size(); project++) {
      if (project_templates[project] != NULL) {
        TemplateButton(project_templates[project]);
      }
    }
    CherryGUI::EndChild();

    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::BeginChild("right", ImVec2(330, 0));

    if (!selected_template_object) {
      auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
      auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));

      ImVec2 child_size = CherryGUI::GetContentRegionAvail();

      ImVec2 image_pos = ImVec2((child_size.x - texture_size.x) / 2.0f, (child_size.y - texture_size.y) / 2.0f);

      CherryGUI::SetCursorPos(image_pos);

      CherryGUI::Image(texture, texture_size);
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#44444466"));

      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;
      static bool advanced_params = false;

      CherryStyle::RemoveMarginY(10.0f);

      CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      {
        CherryStyle::RemoveMarginY(20.0f);
        CherryStyle::AddMarginX(8.0f);
        MyButton(selected_template_object->m_logo_path, 50, 50);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
        CherryGUI::BeginChild(_id, ImVec2(0, 60), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        CherryGUI::SetCursorPosY(CherryGUI::GetStyle().ItemSpacing.y);

        CherryKit::TitleSix(selected_template_object->m_proper_name);

        CherryGUI::SameLine();

        Cherry::SetNextComponentProperty("size_x", "12");
        Cherry::SetNextComponentProperty("size_y", "12");
        Cherry::SetNextComponentProperty("padding_x", "4");
        Cherry::SetNextComponentProperty("padding_y", "2");
        CherryStyle::RemoveMarginY(2.0f);
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/settings.png")).GetData("isClicked") == "true") {
          advanced_params = !advanced_params;
        }

        CherryGUI::EndChild();
      }

      // Divider
      CherryStyle::RemoveMarginY(30.0f);
      CherryGUI::Separator();
      CherryStyle::AddMarginX(10.0f);
      Cherry::SetNextComponentProperty("color_text", "#B1FF31");

      std::string project_type;
      if (selected_template_object->m_type == "project") {
        project_type = Cherry::GetLocale("loc.project_template");
      } else if (selected_template_object->m_type == "tool") {
        project_type = Cherry::GetLocale("loc.tool_template");
      } else {
        project_type = Cherry::GetLocale("loc.template");
      }

      CherryGUI::BeginChild("aboutchild", ImVec2(0, 60), false, ImGuiWindowFlags_NoBackground);  // cherry api

      CherryStyle::AddMarginX(5.0f);
      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TitleSix(project_type);
      CherryStyle::AddMarginX(5.0f);
      Cherry::SetNextComponentProperty("color_text", "#797979");
      CherryKit::TextWrapped(selected_template_object->m_description);

      CherryGUI::EndChild();

      if (advanced_params) {
        CherryGUI::Separator();
        Cherry::SetNextComponentProperty("header_visible", "false");
        CherryKit::TableSimple(
            "Project props inputs",
            {
                CherryKit::KeyValString(Cherry::GetLocale("loc.name"), &v_ProjectName),
                CherryKit::KeyValString(Cherry::GetLocale("loc.description"), &v_ProjectDescription),
                CherryKit::KeyValString(Cherry::GetLocale("loc.author"), &v_ProjectAuthor),
                CherryKit::KeyValString(Cherry::GetLocale("loc.version"), &v_ProjectVersion),
                CherryKit::KeyValComboString(
                    CherryID("VersionSelector"),
                    Cherry::GetLocale("loc.vortex_version"),
                    &selected_template_object->m_compatible_versions),
                CherryKit::KeyValComboString(
                    CherryID("PathSelector"),
                    Cherry::GetLocale("loc.project_path"),
                    &projectPoolsPaths),  // TODO : Take this by the index, not the cherry datas
            });
      }

      CherryKit::Space(20.0f);

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 windowPos = CherryGUI::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + CherryGUI::GetStyle().ItemSpacing.y * 2;

      CherryGUI::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      CherryGUI::Separator();

      CherryStyle::AddMarginX(10.0f);
      Cherry::SetNextComponentProperty("padding_y", "6.5");
      Cherry::SetNextComponentProperty("size_x", std::to_string(CherryGUI::GetContentRegionAvail().x - 100.0f));
      CherryKit::InputString("", &v_ProjectName);
      CherryGUI::SameLine();
      if (CherryKit::ButtonImageText(Cherry::GetLocale("loc.create"), Cherry::GetPath("resources/base/add.png"))
              .GetData("isClicked") == "true") {
        std::string vx_version;

        if (Cherry::GetData(CherryID("VersionSelector"), "selected_string") != "undefined") {
          vx_version = Cherry::GetData(CherryID("VersionSelector"), "selected_string");
        } else {
          if (!selected_template_object->m_compatible_versions.empty()) {
            vx_version = selected_template_object->m_compatible_versions.back();
          } else {
            vx_version = "Undefined";
          }
        }
        if (Cherry::GetData(CherryID("PathSelector"), "selected_string") != "undefined") {
#ifdef _WIN32
          std::string creation_path = Cherry::GetData(CherryID("PathSelector"), "selected_string") + "\\" + v_ProjectName;
          creation_path = VortexMaker::convertPathToWindowsStyle(creation_path);
#else
          std::string creation_path = Cherry::GetData(CherryID("PathSelector"), "selected_string") + "/" + v_ProjectName;
#endif

#ifdef _WIN32
          std::string image_path = "\\icon.png";
#else
          std::string image_path = "/icon.png";
#endif

          VortexMaker::CreateProject(
              v_ProjectName,
              v_ProjectAuthor,
              vx_version,
              v_ProjectVersion,
              v_ProjectDescription,
              creation_path,
              creation_path + image_path,
              selected_template_object->m_name);
          project_blocks.clear();
          VortexMaker::RefreshEnvironmentProjects();
        } else {
#ifdef _WIN32
          std::string creation_path = projectPoolsPaths.back() + "\\" + v_ProjectName + "\\";
          creation_path = VortexMaker::convertPathToWindowsStyle(creation_path);
#else
          std::string creation_path = projectPoolsPaths.back() + "/" + v_ProjectName + "/";
#endif

#ifdef _WIN32
          std::string image_path = "\\icon.png";
#else
          std::string image_path = "/icon.png";
#endif

          if (!projectPoolsPaths.empty()) {
            VortexMaker::CreateProject(
                v_ProjectName,
                v_ProjectAuthor,
                vx_version,
                v_ProjectVersion,
                v_ProjectDescription,
                creation_path,
                creation_path + image_path,
                selected_template_object->m_name);
            project_blocks.clear();
            VortexMaker::RefreshEnvironmentProjects();
          } else {
            std::cout << "Unable to create a project, no project pools are founded !" << std::endl;
          }
        }
      }

      CherryGUI::SameLine();
      CherryGUI::PopStyleColor();
    }
    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void WelcomeWindow::OpenProjectRender() {
    float bottom_pan = 130.0f;
    float avail_x = CherryGUI::GetContentRegionAvail().x;
    float avail_y = CherryGUI::GetContentRegionAvail().y - bottom_pan;
    CherryGUI::BeginChild(
        "left_pane", ImVec2(avail_x, avail_y), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleFive(Cherry::GetLocale("loc.windows.welcome.latest_projects_tools"));
    Cherry::PopFont();

    CherryGUI::SameLine();
    CherryStyle::AddMarginX(10.0f);
    Cherry::SetNextComponentProperty("padding_x", "8");
    Cherry::SetNextComponentProperty("padding_y", "4");

    if (CherryKit::ButtonImageText(Cherry::GetLocale("loc.import"), Cherry::GetPath("resources/base/add.png"))
            .GetData("isClicked") == "true") {
      m_AssetFinder =
          AssetFinder::Create(Cherry::GetLocale("loc.menubar.title.import_projects"), VortexMaker::getHomeDirectory());
      Cherry::ApplicationSpecification spec;

      std::string name = "Import project(s)";
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
      spec.FavIconPath = Cherry::GetPath("resources/imgs/vproject.png");
      spec.IconPath = Cherry::GetPath("resources/imgs/vproject.png");
      spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
      spec.WindowSaves = false;
      spec.MenubarCallback = [this]() {
        if (CherryGUI::BeginMenu(Cherry::GetLocale("loc.window").c_str())) {
          if (CherryGUI::MenuItem(Cherry::GetLocale("loc.close").c_str())) {
            Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
          }
          CherryGUI::EndMenu();
        }
      };

      m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);
      m_AssetFinder->m_ElementName = "";

      if (!VortexMaker::GetCurrentContext()->IO.sys_projects_pools.empty()) {
        m_AssetFinder->m_TargetPossibilities = VortexMaker::GetCurrentContext()->IO.sys_modules_pools;
      }

      m_AssetFinder->GetAppWindow()->SetVisibility(true);
      m_AssetFinder->m_ItemToReconize.push_back(
          std::make_shared<AssetFinderItem>(
              VortexMaker::CheckProjectInDirectory,
              Cherry::GetLocale("loc.project"),
              Cherry::GetLocale("loc.project_tool"),
              Cherry::HexToRGBA("#B1FF31")));

      Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
    }

    if (m_AssetFinder) {
      if (m_AssetFinder->m_GetFileBrowserPath) {
        m_AssetFinder->m_GetFileBrowserPath = false;
        // m_FindedModules.clear();

        std::string pool;
        if (!VortexMaker::GetCurrentContext()->IO.sys_projects_pools[m_AssetFinder->m_TargetPoolIndex].empty()) {
          pool = VortexMaker::GetCurrentContext()->IO.sys_projects_pools[m_AssetFinder->m_TargetPoolIndex];
        }

        for (auto selected : m_AssetFinder->m_Selected) {
          VortexMaker::ImportProject(selected, pool);
        }

        project_blocks.clear();
        VortexMaker::RefreshEnvironmentProjects();
        m_AssetFinder->GetAppWindow()->SetVisibility(false);
        m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
      }
    }

    CherryNextProp("color", "#252525");
    CherryKit::Separator();

    if (VortexMaker::GetCurrentContext()->IO.sys_projects.empty()) {
      if (CherryKit::BlockVerticalCustom(
              []() {},
              100.0f,
              100.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/base/add.png"), 15, 15); },
                  []() { CherryKit::TextCenter("Create new"); },
              },
              200)
              .GetData("isClicked") == "true") {
        // m_ProjectCreation = true;
      }
    }

    project_blocks.clear();
    int index = 0;
    for (auto element : VortexMaker::GetCurrentContext()->IO.sys_projects) {
      index++;
      CherryNextComponent.SetRenderMode(RenderMode::CreateOnly);
      project_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              [this, element]() { m_SelectedEnvproject = element; },
              150.0f,
              115.0f,
              {
                  []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 150.0f); },
                  [element]() {
                    CherryStyle::RemoveMarginY(35.0f);
                    CherryStyle::AddMarginX(10.0f);
                    std::string imgpath = element->logoPath;
#ifdef _WIN32
                    imgpath = VortexMaker::convertPathToWindowsStyle(imgpath);
#endif
                    CherryKit::ImageLocal(imgpath, 40.0f, 40.0f);
                  },
                  [element]() {
                    CherryStyle::AddMarginX(5.0f);
                    Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
                    CherryKit::TextSimple(element->name);
                  },
                  [element]() {
                    Cherry::SetNextComponentProperty("color", "#353535");
                    CherryKit::Separator();
                    CherryStyle::AddMarginX(5.0f);

                    std::string versionpath;
                    bool version_available =
                        VortexMaker::CheckIfVortexVersionUtilityExist(element->compatibleWith, versionpath);
                    if (version_available) {
                      Cherry::SetNextComponentProperty("color_text", "#AAAAAA");
                    } else {
                      if (VortexMaker::CheckVersionAvailibility(element->compatibleWith).version == "") {
                        Cherry::SetNextComponentProperty("color_text", "#EE5555");
                      } else {
                        Cherry::SetNextComponentProperty("color_text", "#EEAA55");
                      }
                    }
                    CherryKit::TextSimple(element->compatibleWith);
                    if (!version_available) {
                      CherryGUI::SameLine();

                      if (VortexMaker::CheckVersionAvailibility(element->compatibleWith).version == "") {
                        CherryKit::TooltipImage(
                            Cherry::GetPath("resources/base/error.png"),
                            "Vortex " + element->compatibleWith + " " +
                                Cherry::GetLocale("loc.windows.welcome.not_installed"));
                        CherryGUI::SameLine();
                        Cherry::SetNextComponentProperty("color_text", "#888888");

                        // CherryKit::TextRight(CherryID("qsd"), "Project");
                      } else {
                        CherryKit::TooltipImage(
                            Cherry::GetPath("resources/base/warn.png"),
                            "Vortex " + element->compatibleWith + " " +
                                Cherry::GetLocale("loc.windows.welcome.not_installed_yet"));
                        CherryGUI::SameLine();
                        Cherry::SetNextComponentProperty("color_text", "#888888");

                        // CherryKit::TextRight(CherryID("qsd"), "Project");
                      }
                      CherryStyle::RemoveMarginX(5.0f);
                    }
                  },
              },
              index));
    }

    CherryKit::GridSimple(150.0f, 150.0f, project_blocks);
    CherryGUI::EndChild();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::BeginChild("###rightpan", ImVec2(0, bottom_pan), false, ImGuiWindowFlags_NoScrollbar);
    if (!m_SelectedEnvproject) {
      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;

      CherryStyle::AddMarginY(7.0f);
      CherryGUI::BeginChild('SADH', ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
      {
        CherryStyle::AddMarginX(8.0f);
        MyButton(Cherry::GetPath("resources/imgs/select.png"), 35, 35);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        CherryStyle::RemoveMarginY(10.0f);
        ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
        CherryGUI::BeginChild(_id, ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.select_a_project"));
        CherryGUI::EndChild();
      }
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#44444466"));

      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;

      CherryStyle::AddMarginY(7.0f);
      CherryGUI::BeginChild('SADH', ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
      {
        CherryStyle::AddMarginX(8.0f);
        MyButton(m_SelectedEnvproject->logoPath, 35, 35);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        CherryStyle::RemoveMarginY(10.0f);
        ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
        CherryGUI::BeginChild(_id, ImVec2(0, 35), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        CherryKit::TitleSix(m_SelectedEnvproject->name);
        CherryGUI::EndChild();
      }

      // Divider
      CherryStyle::RemoveMarginY(15.0f);
      CherryGUI::Separator();
      Cherry::SetNextComponentProperty("color_text", "#B1FF31");

      std::string project_type;
      if (m_SelectedEnvproject->type == "project") {
        project_type = Cherry::GetLocale("loc.project");
      } else if (m_SelectedEnvproject->type == "tool") {
        project_type = Cherry::GetLocale("loc.tool");
      } else {
        project_type = Cherry::GetLocale("loc.project");
      }

      CherryGUI::BeginHorizontal("InfoBar");

      CherryStyle::AddMarginX(5.0f);
      CherryKit::TitleSix(project_type);

      ImVec2 p1 = ImVec2(CherryGUI::GetCursorScreenPos().x, CherryGUI::GetCursorScreenPos().y - 5.0f);
      ImVec2 p2 = ImVec2(p1.x, p1.y + 25);
      CherryGUI::GetWindowDrawList()->AddLine(p1, p2, Cherry::HexToImU32("#343434"));
      CherryGUI::Dummy(ImVec2(1, 0));

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TitleSix(m_SelectedEnvproject->compatibleWith);

      CherryGUI::EndHorizontal();

      CherryGUI::BeginChild("aboutchild", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground);  // cherry api

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryStyle::AddMarginX(5.0f);
      CherryKit::TextSimple(m_SelectedEnvproject->description);

      std::string text = Cherry::GetLocale("loc.delete") + " " + Cherry::GetLocale("loc.open");
      ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
      CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);

      CherryNextProp("color_text", "#B1FF31");
      if (CherryKit::ButtonText(Cherry::GetLocale("loc.delete")).GetData("isClicked") == "true") {
        m_SelectedEnvprojectToRemove = m_SelectedEnvproject;
        open_deletion_modal = true;
      }

      CherryGUI::SameLine();

      Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
      Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
      Cherry::SetNextComponentProperty("color_text", "#121212FF");
      if (CherryKit::ButtonText(Cherry::GetLocale("loc.open")).GetData("isClicked") == "true") {
        std::string versionpath;
        bool version_exist =
            VortexMaker::CheckIfVortexVersionUtilityExist(m_SelectedEnvproject->compatibleWith, versionpath);
        if (!version_exist) {
          no_installed_version = m_SelectedEnvproject->compatibleWith;
          no_installed_project_name = m_SelectedEnvproject->name;
          no_installed_project_picture = m_SelectedEnvproject->logoPath;

          no_installed_version_available = VortexMaker::CheckVersionAvailibility(m_SelectedEnvproject->compatibleWith);

          no_installed_modal_opened = true;
        } else {
          if (VortexMaker::CheckIfProjectRunning(m_SelectedEnvproject->path)) {
            already_running_modal_opened = true;
          } else {
            auto sys_versions = VortexMaker::GetAllSystemVersions(m_SelectedEnvproject->compatibleWith);
            if (sys_versions.size() == 1) {
              std::thread([this, sys_versions]() {
                VortexMaker::OpenProject(m_SelectedEnvproject->path, sys_versions.front()->name);
              }).detach();
            } else {
              all_versions_for_project = sys_versions;
              multiple_versions_modal_opened = true;
            }
          }
        }
      }
      CherryGUI::EndChild();

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 windowPos = CherryGUI::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + CherryGUI::GetStyle().ItemSpacing.y * 2;

      CherryGUI::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      CherryGUI::Separator();
      CherryGUI::Spacing();

      CherryGUI::EndChild();

      CherryGUI::PopStyleColor();
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void WelcomeWindow::WelcomeRender() {
    if (VortexMaker::GetCurrentContext()->web_fetched) {
      CherryNextComponent.SetProperty("color_border", "#343434");
      CherryNextComponent.SetProperty("color_bg", "#232323");
      CherryKit::NotificationButton(
          &VortexMaker::GetCurrentContext()->launcher_update_available,
          10,
          "info",
          Cherry::GetLocale("loc.windows.welcome.update_launcher_title"),
          Cherry::GetLocale("loc.windows.welcome.update_launcher_description") +
              VortexMaker::GetCurrentContext()->latest_launcher_version.version,
          []() {
            if (CherryKit::ButtonImageText(
                    Cherry::GetLocale("loc.update_now"), Cherry::GetPath("resources/imgs/icons/misc/icon_upgrade.png"))
                    .GetData("isClicked") == "true") {
              std::thread([]() {
                VortexMaker::OpenLauncherUpdater(
                    VortexMaker::GetCurrentContext()->m_VortexLauncherPath,
                    VortexMaker::GetCurrentContext()->IO.sys_vortexlauncher_dist);
              }).detach();
              Cherry::Application::Get().Close();
            }
          });

      if (VortexMaker::GetCurrentContext()->latest_vortex_version) {
        CherryKit::NotificationSimple(
            &VortexMaker::GetCurrentContext()->vortex_update_available,
            10,
            "info",
            "Vortex " + VortexMaker::GetCurrentContext()->latest_vortex_version->version + " " +
                Cherry::GetLocale("loc.is_live"),
            Cherry::GetLocale("loc.menubar.menuitem.install_vortex_desc") +
                VortexMaker::GetCurrentContext()->latest_vortex_version->version);
      }
    }

    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    {
      float x = CherryGUI::GetContentRegionAvail().x;
      float y = x / 4.726f;
      CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/vortex_banner.png"), x, y);
      CherryStyle::AddMarginX(20.0f);
      CherryStyle::RemoveMarginY(60.0f);
      Cherry::PushFont("ClashMedium");
      CherryNextProp("color_text", "#FFFFFF");
      CherryKit::TitleOne(Cherry::GetLocale("loc.windows.welcome.title"));
      Cherry::PopFont();
    }

    /*Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#BBBBBB");
    CherryKit::TitleOne(Cherry::GetLocale("loc.windows.welcome.overview"));
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryNextProp("color", "#252525");
    CherryKit::Separator();*/

    CherryKit::Space(25.0f);

    // CherryStyle::AddMarginY(20.0f);
    // CherryStyle::AddMarginX(8.0f);
    // CherryNextProp("color_text", "#797979");
    // CherryKit::TitleFour("Fast actions");

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.latest_projects_tools"));
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> blocks;

    if (blocks.empty()) {
      auto recentProjects = VortexMaker::GetRecentProjects(4);

      int i = 0;
      for (auto project : recentProjects) {
        if (project) {
          CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
          blocks.push_back(
              CherryKit::BlockVerticalCustom(
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
                        CherryStyle::RemoveMarginY(5.0f);
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
          str1 = Cherry::GetLocale("loc.windows.welcome.no_project_yet");
          str2 = Cherry::GetLocale("loc.windows.welcome.create_one");
        } else {
          str1 = Cherry::GetLocale("loc.windows.welcome.no_more_project");
          str2 = Cherry::GetLocale("loc.windows.welcome.create_another");
        }
        CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
        blocks.push_back(
            CherryKit::BlockVerticalCustom(
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

    CherryStyle::AddMarginX(8.0f);
    CherryKit::GridSimple(200.0f, 200.0f, blocks);

    CherryKit::Space(5.0f);

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.fast_actions"));
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> actions_blocks;
    std::vector<Cherry::Component> actions_blocks_bottom;

    if (actions_blocks.empty()) {
      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              [this]() { m_SelectedChildName = "?loc:loc.windows.welcome.create_project"; },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/add.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    CherryNextProp("color_text", "#FFFFFF");
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.create_a_project"));
                  },
              },
              1));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              [this]() { m_SelectedChildName = "?loc:loc.windows.welcome.open_project"; },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/open.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    CherryNextProp("color_text", "#FFFFFF");
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.open_a_project"));
                  },
              },
              2));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              m_SettingsCallback,
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/settings.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    CherryNextProp("color_text", "#FFFFFF");
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.settings_confs"));
                  },
              },
              3));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks_bottom.push_back(
          CherryKit::BlockVerticalCustom(
              []() { VortexMaker::OpenURL("https://vortex.infinite.si/learn"); },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/icons/launcher/docs.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    CherryNextProp("color_text", "#FFFFFF");
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.learn_docs"));
                  },
              },
              4));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks_bottom.push_back(
          CherryKit::BlockVerticalCustom(
              []() { VortexMaker::OpenURL("https://forums.infinite.si/"); },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() {
                    CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/icons/launcher/forum.png"), 30, 30);
                  },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    CherryNextProp("color_text", "#FFFFFF");
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.forums"));
                  },
              },
              5));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks_bottom.push_back(
          CherryKit::BlockVerticalCustom(
              []() { VortexMaker::OpenURL("https://vortex.infinite.si/news"); },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/icons/launcher/web.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    CherryNextProp("color_text", "#FFFFFF");
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.whats_news"));
                  },
              },
              6));
    }

    // Draw grid with blocks
    CherryStyle::AddMarginX(8.0f);
    CherryKit::GridSimple(270.0f, 270.0f, actions_blocks);
    CherryStyle::AddMarginX(8.0f);
    CherryKit::GridSimple(270.0f, 270.0f, actions_blocks_bottom);

    CherryKit::Space(8.0f);
    CherryNextProp("color", "#222222");

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.latest_news"));
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> news_blocks;

    static bool was_disconnected;
    static bool refresh_grid_needed = false;

    if (!VortexMaker::GetCurrentContext()->web_fetched) {
      was_disconnected = true;
      if (news_blocks.empty()) {
        CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
        auto block = CherryKit::BannerImageContext(
            402.0f, 140.0f, Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"), "", "");
        news_blocks.push_back(block);
        news_blocks.push_back(block);
      }
    } else {
      if (was_disconnected) {
        news_blocks.clear();
        was_disconnected = false;
        refresh_grid_needed = true;
      }

      if (news_blocks.empty()) {
        for (const auto &article : VortexMaker::GetCurrentContext()->IO.news) {
          if (!article.image_link.empty() &&
              (EndsWith(article.image_link, ".png") || EndsWith(article.image_link, ".jpg")) &&
              (article.image_link.find("http://") == 0 || article.image_link.find("https://") == 0)) {
            CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
            auto block = CherryKit::BannerImageContext(
                402.0f, 140.0f, Cherry::GetHttpPath(article.image_link), article.title, article.description);
            news_blocks.push_back(block);
          }
        }
      }
    }

    CherryStyle::AddMarginX(8.0f);

    if (!refresh_grid_needed) {
      CherryKit::GridSimple(400.0f, 400.0f, news_blocks);
    } else {
      refresh_grid_needed = false;
    }

    /*CherryKit::Space(20.0f);
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
                      CherryStyle::RemoveMarginY(5.0f);
                      CherryStyle::PushFontSize(0.70f);
                      CherryKit::TextSimple(version.already_installed);
                      CherryStyle::PopFontSize();
                  ),
  });
  last_versions_blocks.push_back(block);

  version_index++;
}
}
}

CherryKit::GridSimple(150.0f, 150.0f, &last_versions_blocks);
*/
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
    RefreshTemplates();
    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
    loadProjects(projectPoolsPaths, path + "/projects_pools.json");

    /*   this->AddChild(
           "Support Us",
           WelcomeWindowChild(
               [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/heart.png"), "https://fund.infinite.si/"));
       this->AddChild(
           "What's news",
           WelcomeWindowChild(
               [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/web.png"), "https://vortex.infinite.si/news"));
       this->AddChild(
           "Forums",
           WelcomeWindowChild(
               [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/forum.png"),
       "https://forums.infinite.si/vortex")); this->AddChild( "Documentation", WelcomeWindowChild( [this]() { },
       Cherry::GetPath("resources/imgs/icons/launcher/docs.png"), "https://vortex.infinite.si/learn"));
   */
    this->AddChild(
        "?loc:loc.windows.welcome.overview",
        WelcomeWindowChild(
            [this]() { this->WelcomeRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));
    this->AddChild(
        "?loc:loc.windows.welcome.open_project",
        WelcomeWindowChild(
            [this]() { this->OpenProjectRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));
    this->AddChild(
        "?loc:loc.windows.welcome.create_project",
        WelcomeWindowChild(
            [this]() { this->CreateProjectRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));

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
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, NULL);

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
    CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner.png")), ImVec2(280, 142));

    const float input_width = leftPaneWidth - 17.0f;
    const float header_width = leftPaneWidth - 27.0f;

    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));

    CherryKit::Space(3.0f);
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.overview").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      m_SelectedChildName = "?loc:loc.windows.welcome.overview";
    }

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.create_projects").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      m_SelectedChildName = "?loc:loc.windows.welcome.create_project";
    }

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.your_projects").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      m_SelectedChildName = "?loc:loc.windows.welcome.open_project";
    }

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.settings").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      if (m_SettingsCallback) {
        m_SettingsCallback();
      }
    }
    CherryGUI::PopStyleColor(4);
    CherryGUI::PopStyleVar();

    // CherryStyle::SetPadding(7.0f);

    /*  for (const auto &child : m_Childs) {
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

        if (child.second.WebLink == "undefined") {
          CherryNextProp("color_bg", "#00000000");
          CherryNextProp("color_border", "#00000000");
          CherryNextProp("padding_x", "2");
          CherryNextProp("padding_y", "2");
          CherryNextProp("size_x", "20");
          CherryNextProp("size_y", "20");
          CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
          CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath);
          // Open other tabs...
        } else {
          CherryNextProp("color_bg", "#00000000");
          CherryNextProp("color_border", "#00000000");
          CherryNextProp("padding_x", "2");
          CherryNextProp("padding_y", "2");
          CherryNextProp("size_x", "20");
          CherryNextProp("size_y", "20");
          CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
          if (CherryKit::ButtonImageTextImage(
                  CherryID(child_name),
                  child_name.c_str(),
                  child.second.LogoPath,
                  Cherry::GetPath("resources/imgs/weblink.png"))
                  .GetData("isClicked") == "true") {
            VortexMaker::OpenURL(child.second.WebLink);
          }
        }

        // if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
      }
  */

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(4);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    // CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);
    static std::string prev_child;
    static std::unordered_map<std::string, float> scroll_memory;

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

      ImGuiWindowFlags child_flags = 0;

      if (m_SelectedChildName == "?loc:loc.windows.welcome.open_project") {
        child_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
      } else {
        child_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
      }

      if (CherryGUI::BeginChild("ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | child_flags)) {
        if (prev_child != m_SelectedChildName) {
          if (!prev_child.empty()) {
            scroll_memory[prev_child] = CherryGUI::GetScrollY();
          }

          if (m_SelectedChildName == "?loc:loc.windows.welcome.open_project") {
            CherryGUI::SetScrollY(0.0f);
          }

          else if (m_SelectedChildName == "?loc:loc.windows.welcome.overview") {
            auto it = scroll_memory.find(m_SelectedChildName);
            if (it != scroll_memory.end()) {
              CherryGUI::SetScrollY(it->second);
            }
          }
        }

        if (auto child = GetChild(m_SelectedChildName)) {
          if (child->RenderCallback) {
            child->RenderCallback();
          }
        }
      }
      CherryGUI::EndChild();

      CherryGUI::PopStyleVar(2);

      prev_child = m_SelectedChildName;
    }

    CherryGUI::EndGroup();

    static int selected_version_index = 0;

    static bool user_string_validation = false;
    static char string_validation[256] = "";
    if (open_deletion_modal) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.delete_project").c_str());

      if (CherryGUI::BeginPopupModal(Cherry::GetLocale("loc.windows.welcome.delete_project").c_str(), NULL, NULL)) {
        static char path_input_all[512];
        Cherry::SetNextComponentProperty("color_text", "#CC2222");
        CherryKit::TitleThree(m_SelectedEnvprojectToRemove->name);
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.project_delete_warning").c_str());
        CherryGUI::InputText("####name_validation", string_validation, sizeof(string_validation));

        std::string text = CherryApp.GetLocale("loc.delete") + CherryApp.GetLocale("loc.close");
        ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
        CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 35.0f);
        CherryNextProp("color", "#222222");
        CherryKit::Separator();
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 40);

        CherryNextProp("color_text", "#B1FF31");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.close")).GetData("isClicked") == "true") {
          open_deletion_modal = false;
          CherryGUI::CloseCurrentPopup();
        }

        CherryGUI::SameLine();
        Cherry::SetNextComponentProperty("color_bg", "#ed5247");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#eda49f");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");

        if (strcmp(string_validation, m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
          CherryGUI::BeginDisabled();
        }

        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.delete")).GetData("isClicked") == "true") {
          // Delete
          VortexMaker::DeleteProject(m_SelectedEnvprojectToRemove->path, m_SelectedEnvprojectToRemove->name);

          VortexMaker::RefreshEnvironmentProjects();
          project_blocks.clear();

          open_deletion_modal = false;
          CherryGUI::CloseCurrentPopup();
        }

        if (strcmp(string_validation, m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
          CherryGUI::EndDisabled();
        }

        CherryGUI::EndPopup();
      }
    }

    if (already_running_modal_opened) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.project_already_running").c_str());

      ImVec2 main_window_size = CherryGUI::GetWindowSize();
      ImVec2 window_pos = CherryGUI::GetWindowPos();

      CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

      CherryGUI::SetNextWindowSize(ImVec2(400, 170), ImGuiCond_Always);
      if (CherryGUI::BeginPopupModal(
              Cherry::GetLocale("loc.windows.welcome.project_already_running").c_str(),
              NULL,
              ImGuiWindowFlags_AlwaysAutoResize)) {
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.project_already_running_description").c_str());

        std::string text = CherryApp.GetLocale("loc.open") + CherryApp.GetLocale("loc.close");
        ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
        CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 35.0f);
        CherryNextProp("color", "#222222");
        CherryKit::Separator();
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 40);

        CherryNextProp("color_text", "#B1FF31");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.close")).GetData("isClicked") == "true") {
          CherryGUI::CloseCurrentPopup();
          already_running_modal_opened = false;
        }

        CherryGUI::SameLine();

        Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.open")).GetData("isClicked") == "true") {
          auto sys_versions = VortexMaker::GetAllSystemVersions(m_SelectedEnvproject->compatibleWith);
          if (sys_versions.size() == 1) {
            std::thread([this, sys_versions]() {
              VortexMaker::OpenProject(m_SelectedEnvproject->path, sys_versions.front()->name);
            }).detach();
          } else {
            all_versions_for_project = sys_versions;
            multiple_versions_modal_opened = true;
          }

          CherryGUI::CloseCurrentPopup();
          already_running_modal_opened = false;
        }

        CherryGUI::EndPopup();
      }
    }
    if (multiple_versions_modal_opened) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.select_a_verson").c_str());

      ImVec2 main_window_size = CherryGUI::GetWindowSize();
      ImVec2 window_pos = CherryGUI::GetWindowPos();

      CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

      CherryGUI::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
      if (CherryGUI::BeginPopupModal(
              Cherry::GetLocale("loc.windows.welcome.select_a_verson").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.select_a_verson_description").c_str());

        if (all_versions_for_project.empty()) {
          CherryGUI::Text(Cherry::GetLocale("loc.windows.welcome.no_versions_available").c_str());
        } else {
          std::vector<const char *> version_names;
          version_names.reserve(all_versions_for_project.size());

          for (const auto &v : all_versions_for_project) {
            version_names.push_back(v->name.c_str());
          }

          CherryGUI::Combo("##VersionSelector", &selected_version_index, version_names.data(), version_names.size());
        }

        CherryGUI::Spacing();

        std::string text = CherryApp.GetLocale("loc.open") + CherryApp.GetLocale("loc.close");
        ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
        CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 35.0f);
        CherryNextProp("color", "#222222");
        CherryKit::Separator();
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 40);

        CherryNextProp("color_text", "#B1FF31");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.close")).GetData("isClicked") == "true") {
          CherryGUI::CloseCurrentPopup();
          multiple_versions_modal_opened = false;
        }

        CherryGUI::SameLine();

        Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.open")).GetData("isClicked") == "true") {
          if (!all_versions_for_project.empty()) {
            auto selected_version = all_versions_for_project[selected_version_index];
            std::thread([this, selected_version]() {
              VortexMaker::OpenProject(m_SelectedEnvproject->path, selected_version->name);
            }).detach();
          }

          CherryGUI::CloseCurrentPopup();
          multiple_versions_modal_opened = false;
        }

        CherryGUI::EndPopup();
      }
    }

    if (no_installed_modal_opened) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.no_version").c_str());

      ImVec2 main_window_size = CherryGUI::GetWindowSize();
      ImVec2 window_pos = CherryGUI::GetWindowPos();

      CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

      CherryGUI::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);

      if (CherryGUI::BeginPopupModal(
              Cherry::GetLocale("loc.windows.welcome.no_version").c_str(),
              NULL,
              ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
        CherryKit::TitleFour(Cherry::GetLocale("loc.windows.welcome.no_version").c_str());  // B1FF31FF
        {
          std::string text_label = Cherry::GetLocale("loc.windows.welcome.no_version_1") + " \"" +
                                   no_installed_project_name + "\"" + Cherry::GetLocale("loc.windows.welcome.no_version_2") +
                                   +"\"" + no_installed_version + "\"" +
                                   Cherry::GetLocale("loc.windows.welcome.no_version_1");
          CherryGUI::TextWrapped(text_label.c_str());
        }
        CherryGUI::Separator();

        if (no_installed_version_available.version != "") {
          CherryKit::TitleFive(Cherry::GetLocale("loc.all_projects"));  // 797979FF

          {
            // LOGO Section
            CherryGUI::BeginChild(
                "LOGO_", ImVec2(160, 40), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
            MyButton(Cherry::GetHttpPath(no_installed_version_available.banner), 120, 40);
            CherryGUI::EndChild();
            CherryGUI::SameLine();
          }
          {
            // Project Info Section
            ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
            CherryGUI::BeginChild(_id, ImVec2(0, 50), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            CherryGUI::SetCursorPosY(CherryGUI::GetStyle().ItemSpacing.y);
            {
              float fontScale = 0.9f;
              float oldFontSize = CherryGUI::GetFont()->Scale;
              CherryGUI::GetFont()->Scale = fontScale;
              CherryGUI::PushFont(CherryGUI::GetFont());

              CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), no_installed_version_available.name.c_str());

              CherryGUI::GetFont()->Scale = oldFontSize;
              CherryGUI::PopFont();
            }

            // Space(2.0f);
            {
              float fontScale = 0.8f;
              float oldFontSize = CherryGUI::GetFont()->Scale;
              CherryGUI::GetFont()->Scale = fontScale;
              CherryGUI::PushFont(CherryGUI::GetFont());

              CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Version: ");
              CherryGUI::SameLine();
              CherryGUI::TextColored(ImVec4(1.0f, 0.8f, 0.8f, 0.8f), no_installed_version_available.version.c_str());

              CherryGUI::GetFont()->Scale = oldFontSize;
              CherryGUI::PopFont();
            }

            CherryGUI::EndChild();
          }

          CherryGUI::Separator();

          CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.can_install_version").c_str());
        } else {
          if (!VortexMaker::GetCurrentContext()->disconnected) {
            CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.cannot_find_version").c_str());
          } else {
            CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.cannot_find_version_offline").c_str());
          }
        }

        CherryGUI::Separator();

        if (CherryGUI::Button(Cherry::GetLocale("loc.close").c_str())) {
          CherryGUI::CloseCurrentPopup();
          no_installed_modal_opened = false;
        }
        if (no_installed_version_available.version != "") {
          CherryGUI::SameLine();
          CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA("#232323FF"));
          CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#B1FF31FF"));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#FFFFFFFF"));
          CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFFFF"));
          if (CherryGUI::Button(Cherry::GetLocale("loc.install_and_open").c_str())) {
            std::thread([this]() {
              VortexMaker::OpenVortexInstaller(
                  no_installed_version_available.version,
                  no_installed_version_available.arch,
                  no_installed_version_available.dist,
                  no_installed_version_available.plat);
            }).detach();
          }
          CherryGUI::PopStyleColor(4);
        }

        CherryGUI::EndPopup();
      }
    }
  }

}  // namespace VortexLauncher
