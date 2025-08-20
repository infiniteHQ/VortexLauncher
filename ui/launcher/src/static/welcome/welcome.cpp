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
static std::string no_installed_version;
static std::string no_installed_project_name;
static std::string no_installed_project_picture;
static VortexVersion no_installed_version_available;

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
    std::cout << "af" << std::endl;
    float columnWidths[3] = { 140.0f, 250.0f, 150.0f };

    ImVec2 windowSize = CherryGUI::GetWindowSize();

    float availableWidth = windowSize.x - CherryGUI::GetStyle().ItemSpacing.x * 2;

    std::cout << "sdg" << std::endl;
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
    CherryKit::TitleFive("Select a base and create !");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();

    for (int project = 0; project < project_templates.size(); project++) {
      if (project_templates[project] != NULL) {
        TemplateButton(project_templates[project]);
      }
    }
    CherryGUI::EndChild();

    std::cout << "32t5" << std::endl;
    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::BeginChild("right", ImVec2(330, 0));

    std::cout << "sdg" << std::endl;
    if (!selected_template_object) {
      auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
      auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));

      ImVec2 child_size = CherryGUI::GetContentRegionAvail();

      ImVec2 image_pos = ImVec2((child_size.x - texture_size.x) / 2.0f, (child_size.y - texture_size.y) / 2.0f);

      CherryGUI::SetCursorPos(image_pos);

      std::cout << "af" << std::endl;
      CherryGUI::Image(texture, texture_size);
      std::cout << "dsg" << std::endl;
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#44444466"));

      std::cout << "asf" << std::endl;
      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;
      static bool advanced_params = false;

      CherryStyle::RemoveMarginY(10.0f);

      CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      std::cout << "sdg" << std::endl;
      {
        CherryStyle::RemoveMarginY(20.0f);
        CherryStyle::AddMarginX(8.0f);
        MyButton(selected_template_object->m_logo_path, 50, 50);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      std::cout << "jfb" << std::endl;
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

      std::cout << "sg" << std::endl;
      // Divider
      CherryStyle::RemoveMarginY(30.0f);
      CherryGUI::Separator();
      CherryStyle::AddMarginX(10.0f);
      Cherry::SetNextComponentProperty("color_text", "#B1FF31");

      std::string project_type;
      if (selected_template_object->m_type == "project") {
        project_type = "Project Template";
      } else if (selected_template_object->m_type == "tool") {
        project_type = "Tool Template";
      } else {
        project_type = "Template";
      }

      std::cout << "djh" << std::endl;
      CherryGUI::BeginChild("aboutchild", ImVec2(0, 60), false, ImGuiWindowFlags_NoBackground);  // cherry api

      CherryStyle::AddMarginX(5.0f);
      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TitleSix(project_type);
      CherryStyle::AddMarginX(5.0f);
      Cherry::SetNextComponentProperty("color_text", "#797979");
      CherryKit::TextWrapped(selected_template_object->m_description);

      CherryGUI::EndChild();

      std::cout << "sfgkmj" << std::endl;
      if (advanced_params) {
        CherryGUI::Separator();
        Cherry::SetNextComponentProperty("header_visible", "false");
        CherryKit::TableSimple(
            "Project props inputs",
            {
                CherryKit::KeyValString("Name", &v_ProjectName),
                CherryKit::KeyValString("Description", &v_ProjectDescription),
                CherryKit::KeyValString("Author", &v_ProjectAuthor),
                CherryKit::KeyValString("Version", &v_ProjectVersion),
                CherryKit::KeyValComboString(
                    CherryID("VersionSelector"), "Vortex version", &selected_template_object->m_compatible_versions),
                CherryKit::KeyValComboString(
                    CherryID("PathSelector"),
                    "Project path",
                    &projectPoolsPaths),  // TODO : Take this by the index, not the cherry datas
            });
      }

      std::cout << "sdgj" << std::endl;
      CherryKit::Space(20.0f);

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 windowPos = CherryGUI::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + CherryGUI::GetStyle().ItemSpacing.y * 2;

      CherryGUI::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      CherryGUI::Separator();

      std::cout << "v_ProjectName" << v_ProjectName << std::endl;

      CherryStyle::AddMarginX(10.0f);
      Cherry::SetNextComponentProperty("padding_y", "6.5");
      Cherry::SetNextComponentProperty("size_x", std::to_string(CherryGUI::GetContentRegionAvail().x - 100.0f));
      CherryKit::InputString("", &v_ProjectName);
      CherryGUI::SameLine();
      std::cout << "sfgj" << std::endl;
      if (CherryKit::ButtonImageText("Create", Cherry::GetPath("resources/base/add.png")).GetData("isClicked") == "true") {
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

      std::cout << "sdg" << std::endl;
      CherryGUI::SameLine();
      CherryGUI::PopStyleColor();
    }
    std::cout << "a234f" << std::endl;
    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void WelcomeWindow::OpenProjectRender() {
    static bool open_deletion_modal = false;
    float avail_x = CherryGUI::GetContentRegionAvail().x - 350.0f;
    CherryGUI::BeginChild(
        "left_pane", ImVec2(avail_x, 0), true, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

    CherryStyle::AddMarginY(4.0f);
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleFive("All projects and tools");
    Cherry::PopFont();

    CherryGUI::SameLine();
    CherryStyle::AddMarginX(10.0f);
    Cherry::SetNextComponentProperty("padding_x", "8");
    Cherry::SetNextComponentProperty("padding_y", "4");

    if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/base/add.png")).GetData("isClicked") == "true") {
      m_AssetFinder = AssetFinder::Create("Import projects(s)", VortexMaker::getHomeDirectory());
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
        if (CherryGUI::BeginMenu("Window")) {
          if (CherryGUI::MenuItem("Close")) {
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
              VortexMaker::CheckProjectInDirectory, "Project", "Project/Tool", Cherry::HexToRGBA("#B1FF31")));

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

    if (project_blocks.empty()) {
      for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_projects.size(); row++) {
        auto element = VortexMaker::GetCurrentContext()->IO.sys_projects[row];
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
                              "Vortex " + element->compatibleWith +
                                  " is not installed in your system. Please click to see more.");
                          CherryGUI::SameLine();
                          Cherry::SetNextComponentProperty("color_text", "#888888");

                          // CherryKit::TextRight(CherryID("qsd"), "Project");
                        } else {
                          CherryKit::TooltipImage(
                              Cherry::GetPath("resources/base/warn.png"),
                              "Vortex " + element->compatibleWith +
                                  " is not installed in your system but available to download.");
                          CherryGUI::SameLine();
                          Cherry::SetNextComponentProperty("color_text", "#888888");

                          // CherryKit::TextRight(CherryID("qsd"), "Project");
                        }
                        CherryStyle::RemoveMarginX(5.0f);
                      }
                    },
                },
                row));
      }
    }

    CherryKit::GridSimple(CherryID("test"), 150.0f, 150.0f, this->project_blocks);

    CherryGUI::EndChild();

    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::BeginChild("###rightpan", ImVec2(350, 0));
    if (!m_SelectedEnvproject) {
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

      CherryStyle::RemoveMarginY(10.0f);

      CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      {
        CherryStyle::RemoveMarginY(20.0f);
        CherryStyle::AddMarginX(8.0f);
        MyButton(m_SelectedEnvproject->logoPath, 50, 50);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
        CherryGUI::BeginChild(_id, ImVec2(0, 60), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        CherryGUI::SetCursorPosY(CherryGUI::GetStyle().ItemSpacing.y);

        CherryKit::TitleThree(m_SelectedEnvproject->name);

        CherryGUI::EndChild();
      }

      // Divider
      CherryStyle::RemoveMarginY(30.0f);
      CherryGUI::Separator();
      CherryStyle::AddMarginX(10.0f);
      Cherry::SetNextComponentProperty("color_text", "#B1FF31");

      std::string project_type;
      if (m_SelectedEnvproject->type == "project") {
        project_type = "Project";
      } else if (m_SelectedEnvproject->type == "tool") {
        project_type = "Tool";
      } else {
        project_type = "Project";
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

      CherryGUI::Separator();

      CherryGUI::BeginChild("aboutchild", ImVec2(0, 200), true, ImGuiWindowFlags_NoBackground);  // cherry api

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TextSimple(m_SelectedEnvproject->description);

      CherryKit::Space(20.0f);

      CherryGUI::EndChild();

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 windowPos = CherryGUI::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + CherryGUI::GetStyle().ItemSpacing.y * 2;

      CherryGUI::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      CherryGUI::Separator();
      CherryGUI::Spacing();

      float buttonPosX = windowSize.x - buttonSize.x * 2 - CherryGUI::GetStyle().ItemSpacing.x * 3;
      CherryGUI::SetCursorPosX(buttonPosX);

      if (CherryGUI::Button("Delete", buttonSize)) {
        m_SelectedEnvprojectToRemove = m_SelectedEnvproject;
        open_deletion_modal = true;
      }

      CherryGUI::SameLine();

      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#006FFFFF"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#689DFFFF"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#9DBFFFFF"));

      if (CherryGUI::Button("Open Project", buttonSize)) {
        std::string versionpath;
        bool version_exist =
            VortexMaker::CheckIfVortexVersionUtilityExist(m_SelectedEnvproject->compatibleWith, versionpath);
        if (!version_exist) {
          no_installed_version = m_SelectedEnvproject->compatibleWith;
          no_installed_project_name = m_SelectedEnvproject->name;
          no_installed_project_picture = m_SelectedEnvproject->logoPath;

          no_installed_version_available = VortexMaker::CheckVersionAvailibility(
              m_SelectedEnvproject->compatibleWith);  // TODO : In the future, make unique request to api for selected
                                                      // version, and reserve the pagination for all versions page.

          no_installed_modal_opened = true;
        } else {
          if (VortexMaker::CheckIfProjectRunning(m_SelectedEnvproject->path)) {
            CherryGUI::OpenPopup("Project Already Running");
          } else {
            std::thread([this]() {
              VortexMaker::OpenProject(m_SelectedEnvproject->path, m_SelectedEnvproject->compatibleWith);
            }).detach();
          }
        }
      }
      std::cout << "dqssdg" << std::endl;

      if (CherryGUI::BeginPopupModal("Project Already Running", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        CherryGUI::Text("The project seems to have already been launched. Would you like to relaunch a new instance?");

        CherryGUI::Spacing();
        if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::SameLine();
        if (CherryGUI::Button("Continue", ImVec2(120, 0))) {
          std::thread([this]() {
            VortexMaker::OpenProject(m_SelectedEnvproject->path, m_SelectedEnvproject->compatibleWith);
          }).detach();

          CherryGUI::CloseCurrentPopup();
        }

        CherryGUI::EndPopup();
      }

      CherryGUI::PopStyleColor(3);
      CherryGUI::PopStyleColor();
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void WelcomeWindow::WelcomeRender() {
    // CherryKit::TableSimple("", { CherryKit::KeyValString("Project name", &test) });

    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    {
      float x = ImGui::GetContentRegionAvail().x;
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
    CherryKit::TitleSix("Latest projects & tools");
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
          str1 = "No project yet";
          str2 = "Create one :)";
        } else {
          str1 = "No more project";
          str2 = "Create another :)";
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
    CherryKit::TitleSix("Fast actions");
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
                    CherryKit::TextCenter("Create a project");
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
                    CherryKit::TextCenter("Open a project");
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
                    CherryKit::TextCenter("Settings & Configurations");
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
                    CherryKit::TextCenter("Learn, Documentations");
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
                    CherryKit::TextCenter("Forums");
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
                    CherryKit::TextCenter("What's news ?");
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
    CherryKit::TitleSix("Latest news");
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> news_blocks;

    if (VortexMaker::GetCurrentContext()->disconnected) {
      if (news_blocks.empty()) {
        CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
        auto block = CherryKit::BannerImageContext(
            402.0f, 140.0f, Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"), "", "");
        news_blocks.push_back(block);
        news_blocks.push_back(block);
      }
    } else {
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
    CherryKit::GridSimple(400.0f, 400.0f, news_blocks);

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
    std::cout << "dg" << std::endl;
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

    this->AddChild(
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
            [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/forum.png"), "https://forums.infinite.si/vortex"));
    this->AddChild(
        "Documentation",
        WelcomeWindowChild(
            [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/docs.png"), "https://vortex.infinite.si/learn"));

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
    std::cout << "comeWindow::RendercomeWindow::RendercomeWindow::RendercomeWindow::Render" << std::endl;
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

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

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
