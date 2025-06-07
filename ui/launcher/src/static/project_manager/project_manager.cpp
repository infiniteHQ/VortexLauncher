
#include "project_manager.hpp"

#include <algorithm>
#include <cctype>
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

static std::vector<std::shared_ptr<Cherry::Component>> project_blocks;

static void MyButton(
    const std::shared_ptr<EnvProject> envproject,
    std::shared_ptr<EnvProject> &selectedproject,
    int xsize = 100,
    int ysize = 100) {
  ImVec2 squareSize(xsize, ysize);

  std::string versionpath;
  bool version_exist = VortexMaker::CheckIfVortexVersionUtilityExist(envproject->compatibleWith, versionpath);

  const char *originalText = envproject->name.c_str();
  char truncatedText[12];
  const char *versionText = envproject->compatibleWith.c_str();

  if (strlen(originalText) > 8) {
    strncpy(truncatedText, originalText, 8);
    strcpy(truncatedText + 8, "...");
  } else {
    strcpy(truncatedText, originalText);
  }

  ImVec2 textSize = CherryGUI::CalcTextSize(truncatedText);
  ImVec2 totalSize(squareSize.x, squareSize.y + textSize.y + 5);

  ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

  std::string button_id = envproject->name + "squareButtonWithText" + envproject->lastOpened;
  if (CherryGUI::InvisibleButton(button_id.c_str(), totalSize)) {
    selectedproject = envproject;

    if (!version_exist) {
      no_installed_version = envproject->compatibleWith;
      no_installed_project_name = envproject->name;
      no_installed_project_picture = envproject->logoPath;

      no_installed_version_available = VortexMaker::CheckVersionAvailibility(
          envproject->compatibleWith);  // TODO : In the future, make unique request to api for selected version, and reserve
                                        // the pagination for all versions page.

      no_installed_modal_opened = true;
    }
  }

  if (CherryGUI::IsItemHovered()) {
    CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  ImDrawList *drawList = CherryGUI::GetWindowDrawList();

  if (!envproject->logoPath.empty()) {
    drawList->AddImage(
        Cherry::GetTexture(envproject->logoPath), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
  } else {
    drawList->AddImage(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_vortex_default.png")),
        cursorPos,
        ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
  }

  ImVec2 smallRectSize(40, 20);
  ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

  drawList->AddRectFilled(
      smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
  ImVec2 versionTextPos = ImVec2(
      smallRectPos.x + (smallRectSize.x - CherryGUI::CalcTextSize(versionText).x) / 2,
      smallRectPos.y + (smallRectSize.y - CherryGUI::CalcTextSize("version").y) / 2);

  if (version_exist) {
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);
  } else {
    if (VortexMaker::CheckVersionAvailibility(envproject->compatibleWith).version != "") {
      drawList->AddText(versionTextPos, IM_COL32(255, 100, 20, 255), versionText);
    } else {
      drawList->AddText(versionTextPos, IM_COL32(255, 20, 20, 255), versionText);
    }
  }

  ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);

  ImU32 textColor = IM_COL32(255, 255, 255, 255);
  ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
  ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

  if (CherryGUI::IsItemHovered()) {
    CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);

    drawList->AddRect(
        cursorPos,
        ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y),
        IM_COL32(135, 135, 135, 255),
        0.0f,
        0,
        2.0f);
  }

  DrawHighlightedText(drawList, textPos, truncatedText, ProjectSearch, highlightColor, textColor, highlightTextColor);

  float windowVisibleX2 = CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
  if (cursorPos.x + totalSize.x < windowVisibleX2)
    CherryGUI::SameLine();
}

ProjectManager::ProjectManager(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));
  m_AppWindow->SetDefaultBehavior(Cherry::DefaultAppWindowBehaviors::DefaultDocking, "full");

  m_AppWindow->SetClosable(false);

  project_pools = VortexMaker::GetCurrentContext()->IO.sys_projects_pools;
  m_AppWindow->SetLeftMenubarCallback([this]() { this->mainButtonsMenuItem(); });
  m_AppWindow->SetRightMenubarCallback([this]() {
    if (!m_ProjectCreation) {
      if (CherryKit::ButtonImageText("Search Folders", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
              ->GetData("isClicked") == "true") {
        showProjectPools = !showProjectPools;
      }

      if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_refresh.png"))->GetData("isClicked") ==
          "true") {
        project_blocks.clear();
        VortexMaker::RefreshEnvironmentProjects();
      }

      CherryKit::ButtonImageDropdown(
          Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"), [&]() { CherryKit::TitleFive("T"); });
    }
  });

  // Create project components
  /*cp_SimpleTable = Cherry::Application::Get().CreateComponent<Cherry::SimpleTable>("simpletable_2", "KeyvA",
  std::vector<std::string>{"", ""}); cp_SimpleTable->SetHeaderCellPaddingY(12.0f);
  cp_SimpleTable->SetHeaderCellPaddingX(10.0f);
  cp_SimpleTable->SetRowsCellPaddingY(100.0f);

  v_ProjectName = std::make_shared<std::string>("VortexProject");
  cp_ProjectName = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValString>("keyvaldouble_1", v_ProjectName,
  "Project name");

  v_ProjectDescription = std::make_shared<std::string>("This is a amazing description of the project.");
  cp_ProjectDescription = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValString>("keyvaldouble_2",
  v_ProjectDescription, "Description");

  // v_ProjectVersion = std::make_shared<std::string>("1.0.0");
  cp_ProjectVersion = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValSimpleCombo>("keyvaldouble_3",
  std::vector<std::string>(), 0, "Version");

  v_ProjectAuthor = std::make_shared<std::string>("Your team");
  cp_ProjectAuthor = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValString>("keyvaldouble_4",
  v_ProjectAuthor, "Authors");

  // v_ProjectAuthor = std::make_shared<std::string>("Your team");
  loadProjects(projectPoolsPaths, path + "/projects_pools.json");
  cp_ProjectPath = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValSimpleCombo>("keyvaldouble_5",
  projectPoolsPaths, 0, "Select a path");

  v_ProjectOpen = std::make_shared<bool>(true);
  cp_ProjectOpen = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValBoolean>("keyvaldouble_6", v_ProjectOpen,
  "Open after ?");*/

  std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  loadProjects(projectPoolsPaths, path + "/projects_pools.json");

  this->ctx = VortexMaker::GetCurrentContext();
  this->Refresh();
}

void ProjectManager::Render() {
  std::cout << "ProjectManagerProjectManagerProjectManagerProjectManager::RendercomeWindow::RendercomeWindow::"
               "RendercomeWindow::Render"
            << std::endl;

  std::cout << "ProjectManager<b" << std::endl;
  static bool open_deletion_modal = false;
  static bool user_string_validation = false;
  static char string_validation[256] = "";
  static float leftPaneWidth = 825.0f;
  const float minPaneWidth = 50.0f;
  const float splitterWidth = 1.5f;
  static int selected;

  if (open_deletion_modal) {
    if (CherryGUI::BeginPopupModal("Delete a project", NULL, NULL)) {
      static char path_input_all[512];
      Cherry::SetNextComponentProperty("color_text", "#CC2222");
      CherryKit::TitleThree(m_SelectedEnvprojectToRemove->name);
      CherryGUI::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
      CherryGUI::InputText(
          "####Please validate by entering the name of the project.", string_validation, sizeof(string_validation));

      CherryKit::Separator();

      if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
        open_deletion_modal = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::SetItemDefaultFocus();
      CherryGUI::SameLine();

      if (strcmp(string_validation, m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
        CherryGUI::BeginDisabled();
      }

      CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
      if (CherryGUI::Button("Delete", ImVec2(120, 0))) {
        // Delete
        VortexMaker::DeleteProject(m_SelectedEnvprojectToRemove->path, m_SelectedEnvprojectToRemove->name);

        VortexMaker::RefreshEnvironmentProjects();
        project_blocks.clear();

        open_deletion_modal = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::PopStyleColor(3);
      if (strcmp(string_validation, m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
        CherryGUI::EndDisabled();
      }
      CherryGUI::EndPopup();
    }
  }

  if (open_deletion_modal)
    CherryGUI::OpenPopup("Delete a project");

  if (no_installed_modal_opened) {
    CherryGUI::OpenPopup("Not installed version");

    ImVec2 main_window_size = CherryGUI::GetWindowSize();
    ImVec2 window_pos = CherryGUI::GetWindowPos();

    CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

    CherryGUI::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);

    if (CherryGUI::BeginPopupModal(
            "Not installed version",
            NULL,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
      CherryKit::TitleFour("Not installed version");  // B1FF31FF
      {
        std::string text_label = "You wanna open the project \"" + no_installed_project_name +
                                 "\" but this project was created on the Vortex version \"" + no_installed_version +
                                 "\" wich not installed in your system";
        CherryGUI::TextWrapped(text_label.c_str());
      }
      CherryGUI::Separator();

      if (no_installed_version_available.version != "") {
        CherryKit::TitleFive("All projects");  // 797979FF

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

        std::string text_label = "But good news ! We can install the version to open this project.";
        CherryGUI::TextWrapped(text_label.c_str());
      } else {
        if (true)  // Connected to the Vortex version svc
        {
          std::string text_label =
              "Inforunetly, we cannot find this version. Please contact the project creator, or find manually this version "
              "by import it, or find a good.";
          CherryGUI::TextWrapped(text_label.c_str());
        } else {
          std::string text_label =
              "Inforunetly, the web Vortex service not respondding (your connected to internet ?) so we cannot find this "
              "versions  Please contact the project creator, or find manually this version by import it.";
          CherryGUI::TextWrapped(text_label.c_str());
        }
      }

      CherryGUI::Separator();

      if (CherryGUI::Button("Close")) {
        CherryGUI::CloseCurrentPopup();
        no_installed_modal_opened = false;
      }
      if (no_installed_version_available.version != "") {
        CherryGUI::SameLine();
        CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA("#232323FF"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#B1FF31FF"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#FFFFFFFF"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFFFF"));
        if (CherryGUI::Button("Install and Open")) {
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

  if (!m_ProjectCreation) {
    CherryGUI::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleFive("All projects and tools");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");

    CherryGUI::SameLine();
    CherryStyle::AddMarginX(10.0f);
    Cherry::SetNextComponentProperty("padding_x", "8");
    Cherry::SetNextComponentProperty("padding_y", "4");

    if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") == "true") {
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
      m_AssetFinder->m_ItemToReconize.push_back(std::make_shared<AssetFinderItem>(
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

    CherryKit::Separator();

    if (ctx->IO.sys_projects.empty()) {
      if (CherryKit::BlockVerticalCustom(
              100.0f,
              100.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/base/add.png"), 15, 15); },
                  []() { CherryKit::TextCenter("Create new"); },
              },
              200)
              ->GetData("isClicked") == "true") {
        m_ProjectCreation = true;
      }
    }

    if (project_blocks.empty()) {
      for (int row = 0; row < ctx->IO.sys_projects.size(); row++) {
        auto element = ctx->IO.sys_projects[row];
        project_blocks.push_back(CherryKit::BlockVerticalCustom(
            Cherry::ID(Cherry::IdentifierProperty::CreateOnly, element, row),
            [this, element]() { m_SelectedEnvproject = element; },
            150.0f,
            115.0f,
            {
                []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 150.0f); },
                [element]() {
                  CherryStyle::RemoveYMargin(35.0f);
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
                    CherryStyle::RemoveXMargin(5.0f);
                  }
                },
            }));
      }
    }

    CherryKit::GridSimple(150.0f, 150.0f, &project_blocks);

    CherryGUI::EndChild();

    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::BeginChild("###rightpan");
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

      CherryStyle::RemoveYMargin(10.0f);

      CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      {
        CherryStyle::RemoveYMargin(20.0f);
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
      CherryStyle::RemoveYMargin(30.0f);
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

      std::cout << "dqs" << std::endl;
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
  } else {
    float columnWidths[3] = { 140.0f, 250.0f, 150.0f };

    ImVec2 windowSize = CherryGUI::GetWindowSize();

    float availableWidth = windowSize.x - CherryGUI::GetStyle().ItemSpacing.x * 2;

    float totalColumnWidths = 0.0f;
    for (int i = 0; i < 3; ++i)
      totalColumnWidths += columnWidths[i];

    float columnProportions[3];
    for (int i = 0; i < 3; ++i)
      columnProportions[i] = columnWidths[i] / totalColumnWidths;

    static std::vector<std::shared_ptr<Cherry::Component>> pt_blocks;

    if (pt_blocks.empty()) {
      pt_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::ID(Cherry::IdentifierProperty::CreateOnly, "pt_blocks"),
          []() {},
          290.0f,
          60.0f,
          {
              []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 290.0f, 60.0f); },
              []() {
                CherryStyle::RemoveYMargin(45.0f);
                CherryStyle::AddMarginX(15.0f);

                Cherry::PushFont("ClashBold");
                CherryKit::TitleFour("All");
                Cherry::PopFont();
              },
          }));

      pt_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::ID(Cherry::IdentifierProperty::CreateOnly, 1, "pt_blocks"),
          []() {},
          290.0f,
          60.0f,
          {
              []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 290.0f, 60.0f); },
              []() {
                CherryStyle::RemoveYMargin(45.0f);
                CherryStyle::AddMarginX(15.0f);

                Cherry::PushFont("ClashBold");
                CherryKit::TitleFour("Systems");
                Cherry::PopFont();
              },
          }));

      pt_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::ID(Cherry::IdentifierProperty::CreateOnly, 2, "pt_blocks"),
          []() {},
          290.0f,
          60.0f,
          {
              []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 290.0f, 60.0f); },
              []() {
                CherryStyle::RemoveYMargin(45.0f);
                CherryStyle::AddMarginX(15.0f);

                Cherry::PushFont("ClashBold");
                CherryKit::TitleFour("Apps & Services");
                Cherry::PopFont();
              },
          }));

      pt_blocks.push_back(CherryKit::BlockVerticalCustom(
          Cherry::ID(Cherry::IdentifierProperty::CreateOnly, 3, "pt_blocks"),
          []() {},
          290.0f,
          60.0f,
          {
              []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 290.0f, 60.0f); },
              []() {
                CherryStyle::RemoveYMargin(45.0f);
                CherryStyle::AddMarginX(15.0f);

                Cherry::PushFont("ClashBold");
                CherryKit::TitleFour("Tools & Utilities");
                Cherry::PopFont();
              },
          }));
    }

    float total_x = CherryGUI::GetContentRegionAvail().x;

    CherryStyle::AddMarginY(2.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, ImVec2(0, 0));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#101010"));
    CherryGUI::BeginChild("left", ImVec2(290, 0), true);
    CherryKit::GridSimple(CherryID("banner"), 290, 290, &pt_blocks);
    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar();

    CherryGUI::SameLine();

    CherryStyle::AddMarginX(5.0f);
    CherryGUI::BeginChild("###sdgf5mid", ImVec2(total_x - 620, 0), false, ImGuiWindowFlags_NoBackground);
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

      CherryStyle::RemoveYMargin(10.0f);

      CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      {
        CherryStyle::RemoveYMargin(20.0f);
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
        CherryStyle::RemoveYMargin(2.0f);
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/settings.png"))->GetData("isClicked") == "true") {
          advanced_params = !advanced_params;
        }

        CherryGUI::EndChild();
      }

      // Divider
      CherryStyle::RemoveYMargin(30.0f);
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
      CherryKit::InputString("", &v_ProjectName);
      CherryGUI::SameLine();
      if (CherryKit::ButtonImageText("Create", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") == "true") {
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

  static std::vector<std::string> projectPaths;
  static char newPath[256] = "";
  static bool initialized = false;
  if (!initialized) {
    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
    std::string json_file = path + "/projects_pools.json";
    loadProjects(projectPaths, json_file);
    initialized = true;
  }

  if (showProjectPools) {
    CherryGUI::OpenPopup("Manage Project Pools");
    if (CherryGUI::BeginPopupModal(
            "Manage Project Pools",
            NULL,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
      CherryGUI::Text("Path where Vortex search projects:");
      CherryGUI::Separator();

      CherryGUI::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
      if (CherryGUI::BeginTable("##project_paths", 2)) {
        CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

        CherryGUI::TableSetupColumn("Path");
        CherryGUI::TableSetupColumn("Action");
        CherryGUI::TableHeadersRow();

        for (size_t i = 0; i < projectPaths.size(); ++i) {
          CherryGUI::TableNextRow();

          CherryGUI::TableSetColumnIndex(0);
          CherryGUI::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

          CherryGUI::Text("%s", projectPaths[i].c_str());

          CherryGUI::PopStyleColor();

          /*static std::shared_ptr<Cherry::ImageTextButtonSimple> import_btn =
          std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", ""); import_btn->SetScale(0.85f);
          import_btn->SetInternalMarginX(10.0f);
          import_btn->SetLogoSize(15, 15);
          import_btn->SetBackgroundColorIdle("#00000000");
          import_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));
          std::string delete_btn_label = "Delete####" + projectPaths[i];*/

          CherryGUI::TableSetColumnIndex(1);

          if (CherryKit::ButtonImageText(
                  CherryID("delete" + std::to_string(i)), "", Cherry::GetPath("resources/imgs/trash.png"))
                  ->GetData("isClicked") == "true") {
            projectPaths.erase(projectPaths.begin() + i);
            --i;
          }
        }

        CherryGUI::PopStyleVar();
        CherryGUI::EndTable();
      }

      CherryGUI::PopStyleColor();

      /*static std::shared_ptr<Cherry::ImageTextButtonSimple> add_btn =
      std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "####add"); add_btn->SetScale(0.85f);
      add_btn->SetInternalMarginX(10.0f);
      add_btn->SetLogoSize(15, 15);
      add_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));

      static std::shared_ptr<Cherry::ImageTextButtonSimple> save_btn =
      std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "Save"); save_btn->SetScale(0.85f);
      save_btn->SetInternalMarginX(10.0f);
      save_btn->SetLogoSize(15, 15);
      save_btn->SetBackgroundColorIdle("#3232F7FF");
      save_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"));*/

      CherryGUI::Separator();

      if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))->GetData("isClicked") ==
          "true") {
        if (newPath[0] != '\0') {
          projectPaths.push_back(std::string(newPath));
          newPath[0] = '\0';
        }
      }

      CherryGUI::SameLine();

      CherryGUI::InputText("", newPath, sizeof(newPath));

      CherryGUI::Separator();

      if (CherryGUI::Button("Cancel")) {
        CherryGUI::CloseCurrentPopup();
        showProjectPools = false;
      }
      CherryGUI::SameLine();

      if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
              ->GetData("isClicked") == "true") {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        std::string json_file = path + "/projects_pools.json";
        saveProjects(projectPaths, json_file);

        VortexMaker::RefreshEnvironmentProjectsPools();
        VortexMaker::RefreshEnvironmentProjects();
      }
      CherryGUI::EndPopup();
    }
  }

  std::cout << "ProjectManager<E" << std::endl;
}

std::shared_ptr<Cherry::AppWindow> &ProjectManager::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<ProjectManager> ProjectManager::Create(const std::string &name) {
  auto instance = std::shared_ptr<ProjectManager>(new ProjectManager(name));
  instance->SetupRenderCallback();
  return instance;
}

void ProjectManager::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

// Helper functions for menu items

static void handleRefresh() {
  // Behavior
}

static void handleAddToProject(const std::string &name, const std::string &version) {
  // Behavior
}

static void handleFilterBuildRebuild() {
  // Behavior
}

static void handleGlobalBuild() {
  // Behavior
}

static void handleCreateModule() {
  // Behavior
}

static void handleSearch() {
  // Behavior
}

void ProjectManager::addModuleModal() {
  //
}

void ProjectManager::mainButtonsMenuItem() {
  /*static std::shared_ptr<Cherry::ImageTextButtonSimple> create_project_button =
  std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "Create a project");
  create_project_button->SetScale(0.85f);
  create_project_button->SetInternalMarginX(10.0f);
  create_project_button->SetLogoSize(15, 15);
  create_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));*/

  /*static std::shared_ptr<Cherry::ImageTextButtonSimple> import_project_button =
  std::make_shared<Cherry::ImageTextButtonSimple>("import_btn", "Import a project");
  import_project_button->SetScale(0.85f); import_project_button->SetInternalMarginX(10.0f);
  import_project_button->SetLogoSize(15, 15);
  import_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"));*/

  /*static std::shared_ptr<Cherry::ImageTextButtonSimple> open_project_button =
  std::make_shared<Cherry::ImageTextButtonSimple>("open_btn", "Open a project"); open_project_button->SetScale(0.85f);
  open_project_button->SetInternalMarginX(10.0f);
  open_project_button->SetLogoSize(15, 15);
  open_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"));*/

  /*static std::shared_ptr<Cherry::ImageTextButtonSimple> add_pools_btn =
  std::make_shared<Cherry::ImageTextButtonSimple>("add_pools_btn", "Search folders"); add_pools_btn->SetScale(0.85f);
  add_pools_btn->SetInternalMarginX(10.0f);
  add_pools_btn->SetLogoSize(15, 15);
  add_pools_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"));*/

  static std::string v_SearchString;
  // static std::shared_ptr<Cherry::ImageStringInput> input_search = std::make_shared<Cherry::ImageStringInput>("open_btn",
  // v_SearchString, Cherry::GetPath("resources/imgs/icons/misc/icon_search.png"), "####Open a project");

  strncpy(ProjectSearch, v_SearchString.c_str(), sizeof(ProjectSearch) - 1);

  if (!m_ProjectCreation) {
    CherryStyle::PushFontSize(0.85f);
    if (CherryKit::ButtonImageText("Create a project", Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
            ->GetData("isClicked") == "true") {
      m_ProjectCreation = true;
    }

    CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#45454545")), CherryGUI::Separator();
    CherryGUI::PopStyleColor();

    CherryGUI::PushItemWidth(200);
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 6.0f));  // Largeur x Hauteur padding
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#353535FF"));

    // input_search->Render("_");
    CherryKit::InputString("", &v_SearchString);

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor();
    CherryGUI::PopItemWidth();

    CherryStyle::PopFontSize();
    // TODO ProjectSearch = v_SearchString->c_str();
  } else {
    if (CherryKit::ButtonImageText("Open a project", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
            ->GetData("isClicked") == "true") {
      m_ProjectCreation = false;
    }
  }
}

void ProjectManager::filterMenuItem() {
  CherryGUI::Separator();
  if (CherryGUI::BeginMenu("Filters")) {
    if (CherryGUI::MenuItem("Build/Rebuild single parts")) {
      handleFilterBuildRebuild();
    }
    if (CherryGUI::MenuItem("Global build")) {
      handleGlobalBuild();
    }
    CherryGUI::EndMenu();
  }
}

void ProjectManager::createMenuItem() {
  if (CherryGUI::BeginMenu("Create a module")) {
    if (CherryGUI::MenuItem("Build/Rebuild single parts")) {
      handleCreateModule();
    }
    if (CherryGUI::MenuItem("Global build")) {
      handleGlobalBuild();
    }
    CherryGUI::EndMenu();
  }
}

void ProjectManager::searchMenuItem() {
  if (CherryGUI::BeginMenu("Search")) {
    if (CherryGUI::MenuItem("Build/Rebuild single parts")) {
      handleSearch();
    }
    if (CherryGUI::MenuItem("Global build")) {
      handleGlobalBuild();
    }
    CherryGUI::EndMenu();
  }
}

void ProjectManager::Refresh() {
  project_templates.clear();
  for (auto tem : this->ctx->IO.sys_templates) {
    if (tem->m_type == "project") {
      project_templates.push_back(tem);
    }
  }
};