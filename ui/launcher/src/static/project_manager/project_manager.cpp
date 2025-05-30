
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

  ImVec2 textSize = ImGui::CalcTextSize(truncatedText);
  ImVec2 totalSize(squareSize.x, squareSize.y + textSize.y + 5);

  ImVec2 cursorPos = ImGui::GetCursorScreenPos();

  std::string button_id = envproject->name + "squareButtonWithText" + envproject->lastOpened;
  if (ImGui::InvisibleButton(button_id.c_str(), totalSize)) {
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

  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  ImDrawList *drawList = ImGui::GetWindowDrawList();

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
      smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(versionText).x) / 2,
      smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);

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

  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    drawList->AddRect(
        cursorPos,
        ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y),
        IM_COL32(135, 135, 135, 255),
        0.0f,
        0,
        2.0f);
  }

  DrawHighlightedText(drawList, textPos, truncatedText, ProjectSearch, highlightColor, textColor, highlightTextColor);

  float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
  if (cursorPos.x + totalSize.x < windowVisibleX2)
    ImGui::SameLine();
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
  static bool open_deletion_modal = false;
  static bool user_string_validation = false;
  static char string_validation[256] = "";
  static float leftPaneWidth = 825.0f;
  const float minPaneWidth = 50.0f;
  const float splitterWidth = 1.5f;
  static int selected;

  if (open_deletion_modal) {
    if (ImGui::BeginPopupModal("Delete a project", NULL, NULL)) {
      static char path_input_all[512];
      Cherry::SetNextComponentProperty("color_text", "#CC2222");
      CherryKit::TitleThree(m_SelectedEnvprojectToRemove->name);
      ImGui::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
      ImGui::InputText(
          "####Please validate by entering the name of the project.", string_validation, sizeof(string_validation));

      CherryKit::Separator();

      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        open_deletion_modal = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();

      if (strcmp(string_validation, m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
        ImGui::BeginDisabled();
      }

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
      if (ImGui::Button("Delete", ImVec2(120, 0))) {
        // Delete
        VortexMaker::DeleteProject(m_SelectedEnvprojectToRemove->path, m_SelectedEnvprojectToRemove->name);

        VortexMaker::RefreshEnvironmentProjects();
        project_blocks.clear();

        open_deletion_modal = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor(3);
      if (strcmp(string_validation, m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
        ImGui::EndDisabled();
      }
      ImGui::EndPopup();
    }
  }

  if (open_deletion_modal)
    ImGui::OpenPopup("Delete a project");

  if (no_installed_modal_opened) {
    ImGui::OpenPopup("Not installed version");

    ImVec2 main_window_size = ImGui::GetWindowSize();
    ImVec2 window_pos = ImGui::GetWindowPos();

    ImGui::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

    ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);

    if (ImGui::BeginPopupModal(
            "Not installed version",
            NULL,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
      CherryKit::TitleFour("Not installed version");  // B1FF31FF
      {
        std::string text_label = "You wanna open the project \"" + no_installed_project_name +
                                 "\" but this project was created on the Vortex version \"" + no_installed_version +
                                 "\" wich not installed in your system";
        ImGui::TextWrapped(text_label.c_str());
      }
      ImGui::Separator();

      if (no_installed_version_available.version != "") {
        CherryKit::TitleFive("All projects");  // 797979FF

        {
          // LOGO Section
          ImGui::BeginChild("LOGO_", ImVec2(160, 40), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
          MyButton(Cherry::GetHttpPath(no_installed_version_available.banner), 120, 40);
          ImGui::EndChild();
          ImGui::SameLine();
        }
        {
          // Project Info Section
          ImGuiID _id = ImGui::GetID("INFO_PANEL");
          ImGui::BeginChild(_id, ImVec2(0, 50), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
          ImGui::SetCursorPosY(ImGui::GetStyle().ItemSpacing.y);
          {
            float fontScale = 0.9f;
            float oldFontSize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale = fontScale;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), no_installed_version_available.name.c_str());

            ImGui::GetFont()->Scale = oldFontSize;
            ImGui::PopFont();
          }

          // Space(2.0f);
          {
            float fontScale = 0.8f;
            float oldFontSize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale = fontScale;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Version: ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.8f, 0.8f), no_installed_version_available.version.c_str());

            ImGui::GetFont()->Scale = oldFontSize;
            ImGui::PopFont();
          }

          ImGui::EndChild();
        }

        ImGui::Separator();

        std::string text_label = "But good news ! We can install the version to open this project.";
        ImGui::TextWrapped(text_label.c_str());
      } else {
        if (true)  // Connected to the Vortex version svc
        {
          std::string text_label =
              "Inforunetly, we cannot find this version. Please contact the project creator, or find manually this version "
              "by import it, or find a good.";
          ImGui::TextWrapped(text_label.c_str());
        } else {
          std::string text_label =
              "Inforunetly, the web Vortex service not respondding (your connected to internet ?) so we cannot find this "
              "versions  Please contact the project creator, or find manually this version by import it.";
          ImGui::TextWrapped(text_label.c_str());
        }
      }

      ImGui::Separator();

      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
        no_installed_modal_opened = false;
      }
      if (no_installed_version_available.version != "") {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA("#232323FF"));
        ImGui::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#B1FF31FF"));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#FFFFFFFF"));
        ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFFFF"));
        if (ImGui::Button("Install and Open")) {
          std::thread([this]() {
            VortexMaker::OpenVortexInstaller(
                no_installed_version_available.version,
                no_installed_version_available.arch,
                no_installed_version_available.dist,
                no_installed_version_available.plat);
          }).detach();
        }
        ImGui::PopStyleColor(4);
      }

      ImGui::EndPopup();
    }
  }

  if (!m_ProjectCreation) {
    ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleFive("All projects and tools");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");

    ImGui::SameLine();
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
        if (ImGui::BeginMenu("Window")) {
          if (ImGui::MenuItem("Close")) {
            Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
          }
          ImGui::EndMenu();
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
                  CherryKit::ImageLocal(Cherry::GetPath(element->logoPath), 40.0f, 40.0f);
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
                  Cherry::SetNextComponentProperty("color_text", "#888888");
                  CherryKit::TextSimple(element->compatibleWith);
                  ImGui::SameLine();
                  CherryKit::TooltipImage(
                      Cherry::GetPath("resources/base/error.png"),
                      "Vortex " + element->compatibleWith + " is not installed in your system. Please click to see more.");
                  ImGui::SameLine();
                  Cherry::SetNextComponentProperty("color_text", "#888888");
                  CherryStyle::RemoveXMargin(5.0f);
                  // CherryKit::TextRight(CherryID("qsd"), "Project");
                },
            }));
      }
    }

    CherryKit::GridSimple(150.0f, 150.0f, &project_blocks);

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    ImGui::BeginChild("###rightpan");
    if (!m_SelectedEnvproject) {
      auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
      auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));

      ImVec2 child_size = ImGui::GetContentRegionAvail();

      ImVec2 image_pos = ImVec2((child_size.x - texture_size.x) / 2.0f, (child_size.y - texture_size.y) / 2.0f);

      ImGui::SetCursorPos(image_pos);

      ImGui::Image(texture, texture_size);
    } else {
      ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#44444466"));

      ImVec2 child_size = ImGui::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;

      CherryStyle::RemoveYMargin(10.0f);

      ImGui::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      {
        CherryStyle::RemoveYMargin(20.0f);
        CherryStyle::AddMarginX(8.0f);
        MyButton(m_SelectedEnvproject->logoPath, 50, 50);
        ImGui::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        ImGuiID _id = ImGui::GetID("INFO_PANEL");
        ImGui::BeginChild(_id, ImVec2(0, 60), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        ImGui::SetCursorPosY(ImGui::GetStyle().ItemSpacing.y);

        CherryKit::TitleThree(m_SelectedEnvproject->name);

        ImGui::EndChild();
      }

      // Divider
      CherryStyle::RemoveYMargin(30.0f);
      ImGui::Separator();
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

      ImGui::BeginHorizontal("InfoBar");

      CherryStyle::AddMarginX(5.0f);
      CherryKit::TitleSix(project_type);

      ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 5.0f);
      ImVec2 p2 = ImVec2(p1.x, p1.y + 25);
      ImGui::GetWindowDrawList()->AddLine(p1, p2, Cherry::HexToImU32("#343434"));
      ImGui::Dummy(ImVec2(1, 0));

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TitleSix(m_SelectedEnvproject->compatibleWith);

      ImGui::EndHorizontal();

      ImGui::Separator();

      ImGui::BeginChild("aboutchild", ImVec2(0, 200), true, ImGuiWindowFlags_NoBackground);  // cherry api

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TextSimple(m_SelectedEnvproject->description);

      CherryKit::Space(20.0f);

      ImGui::EndChild();

      ImVec2 windowSize = ImGui::GetWindowSize();
      ImVec2 windowPos = ImGui::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + ImGui::GetStyle().ItemSpacing.y * 2;

      ImGui::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      ImGui::Separator();
      ImGui::Spacing();

      float buttonPosX = windowSize.x - buttonSize.x * 2 - ImGui::GetStyle().ItemSpacing.x * 3;
      ImGui::SetCursorPosX(buttonPosX);

      if (ImGui::Button("Delete", buttonSize)) {
        m_SelectedEnvprojectToRemove = m_SelectedEnvproject;
        open_deletion_modal = true;
      }

      ImGui::SameLine();

      ImGui::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#006FFFFF"));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#689DFFFF"));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#9DBFFFFF"));

      if (ImGui::Button("Open Project", buttonSize)) {
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
            ImGui::OpenPopup("Project Already Running");
          } else {
            std::thread([this]() {
              VortexMaker::OpenProject(m_SelectedEnvproject->path, m_SelectedEnvproject->compatibleWith);
            }).detach();
          }
        }
      }

      if (ImGui::BeginPopupModal("Project Already Running", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("The project seems to have already been launched. Would you like to relaunch a new instance?");

        ImGui::Spacing();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
          ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Continue", ImVec2(120, 0))) {
          std::thread([this]() {
            VortexMaker::OpenProject(m_SelectedEnvproject->path, m_SelectedEnvproject->compatibleWith);
          }).detach();

          ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
      }

      ImGui::PopStyleColor(3);
      ImGui::PopStyleColor();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
  } else {
    float columnWidths[3] = { 140.0f, 250.0f, 150.0f };

    ImVec2 windowSize = ImGui::GetWindowSize();

    float availableWidth = windowSize.x - ImGui::GetStyle().ItemSpacing.x * 2;

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

    float total_x = ImGui::GetContentRegionAvail().x;

    CherryStyle::AddMarginY(2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#101010"));
    ImGui::BeginChild("left", ImVec2(290, 0), true);
    CherryKit::GridSimple(CherryID("banner"), 290, 290, &pt_blocks);
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::SameLine();

    CherryStyle::AddMarginX(5.0f);
    ImGui::BeginChild("###sdgf5mid", ImVec2(total_x - 620, 0), false, ImGuiWindowFlags_NoBackground);
    for (int project = 0; project < project_templates.size(); project++) {
      if (project_templates[project] != NULL) {
        TemplateButton(project_templates[project]);
      }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    ImGui::BeginChild("right", ImVec2(330, 0));

    if (!selected_template_object) {
      auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
      auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));

      ImVec2 child_size = ImGui::GetContentRegionAvail();

      ImVec2 image_pos = ImVec2((child_size.x - texture_size.x) / 2.0f, (child_size.y - texture_size.y) / 2.0f);

      ImGui::SetCursorPos(image_pos);

      ImGui::Image(texture, texture_size);
    } else {
      ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#44444466"));

      ImVec2 child_size = ImGui::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;
      static bool advanced_params = false;

      CherryStyle::RemoveYMargin(10.0f);

      ImGui::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(imagex, imagey));

      {
        CherryStyle::RemoveYMargin(20.0f);
        CherryStyle::AddMarginX(8.0f);
        MyButton(selected_template_object->m_logo_path, 50, 50);
        ImGui::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        ImGuiID _id = ImGui::GetID("INFO_PANEL");
        ImGui::BeginChild(_id, ImVec2(0, 60), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        ImGui::SetCursorPosY(ImGui::GetStyle().ItemSpacing.y);

        CherryKit::TitleSix(selected_template_object->m_proper_name);

        ImGui::SameLine();

        Cherry::SetNextComponentProperty("size_x", "12");
        Cherry::SetNextComponentProperty("size_y", "12");
        Cherry::SetNextComponentProperty("padding_x", "4");
        Cherry::SetNextComponentProperty("padding_y", "2");
        CherryStyle::RemoveYMargin(2.0f);
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/settings.png"))->GetData("isClicked") == "true") {
          advanced_params = !advanced_params;
        }

        ImGui::EndChild();
      }

      // Divider
      CherryStyle::RemoveYMargin(30.0f);
      ImGui::Separator();
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

      ImGui::BeginChild("aboutchild", ImVec2(0, 60), false, ImGuiWindowFlags_NoBackground);  // cherry api

      CherryStyle::AddMarginX(5.0f);
      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TitleSix(project_type);
      CherryStyle::AddMarginX(5.0f);
      Cherry::SetNextComponentProperty("color_text", "#797979");
      CherryKit::TextWrapped(selected_template_object->m_description);

      ImGui::EndChild();

      if (advanced_params) {
        ImGui::Separator();
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
                CherryKit::KeyValComboString(CherryID("PathSelector"), "Project path", &projectPoolsPaths),
            });
      }

      CherryKit::Space(20.0f);

      ImVec2 windowSize = ImGui::GetWindowSize();
      ImVec2 windowPos = ImGui::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + ImGui::GetStyle().ItemSpacing.y * 2;

      ImGui::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      ImGui::Separator();

      CherryStyle::AddMarginX(10.0f);
      Cherry::SetNextComponentProperty("padding_y", "6.5");
      CherryKit::InputString("", &v_ProjectName);
      ImGui::SameLine();
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
          std::string path = Cherry::GetData(CherryID("PathSelector"), "selected_string") + "/" + v_ProjectName;
          VortexMaker::CreateProject(
              v_ProjectName,
              v_ProjectAuthor,
              v_ProjectVersion,
              v_ProjectDescription,
              path,
              path + "/icon.png",
              selected_template_object->m_name);
          project_blocks.clear();
          VortexMaker::RefreshEnvironmentProjects();
        } else {
          if (!projectPoolsPaths.empty()) {
            VortexMaker::CreateProject(
                v_ProjectName,
                v_ProjectAuthor,
                v_ProjectVersion,
                v_ProjectDescription,
                projectPoolsPaths.back(),
                projectPoolsPaths.back() + "/icon.png",
                selected_template_object->m_name);
            project_blocks.clear();
            VortexMaker::RefreshEnvironmentProjects();
          } else {
            std::cout << "Unable to create a project, no project pools are founded !" << std::endl;
          }
        }
      }

      ImGui::SameLine();
      ImGui::PopStyleColor();
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
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
    ImGui::OpenPopup("Manage Project Pools");
    if (ImGui::BeginPopupModal(
            "Manage Project Pools",
            NULL,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
      ImGui::Text("Path where Vortex search projects:");
      ImGui::Separator();

      ImGui::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
      if (ImGui::BeginTable("##project_paths", 2)) {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

        ImGui::TableSetupColumn("Path");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < projectPaths.size(); ++i) {
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

          ImGui::Text("%s", projectPaths[i].c_str());

          ImGui::PopStyleColor();

          /*static std::shared_ptr<Cherry::ImageTextButtonSimple> import_btn =
          std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", ""); import_btn->SetScale(0.85f);
          import_btn->SetInternalMarginX(10.0f);
          import_btn->SetLogoSize(15, 15);
          import_btn->SetBackgroundColorIdle("#00000000");
          import_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));
          std::string delete_btn_label = "Delete####" + projectPaths[i];*/

          ImGui::TableSetColumnIndex(1);

          if (CherryKit::ButtonImageText(
                  CherryID("delete" + std::to_string(i)), "", Cherry::GetPath("resources/imgs/trash.png"))
                  ->GetData("isClicked") == "true") {
            projectPaths.erase(projectPaths.begin() + i);
            --i;
          }
        }

        ImGui::PopStyleVar();
        ImGui::EndTable();
      }

      ImGui::PopStyleColor();

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

      ImGui::Separator();

      if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))->GetData("isClicked") ==
          "true") {
        if (newPath[0] != '\0') {
          projectPaths.push_back(std::string(newPath));
          newPath[0] = '\0';
        }
      }

      ImGui::SameLine();

      ImGui::InputText("", newPath, sizeof(newPath));

      ImGui::Separator();

      if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
        showProjectPools = false;
      }
      ImGui::SameLine();

      if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
              ->GetData("isClicked") == "true") {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        std::string json_file = path + "/projects_pools.json";
        saveProjects(projectPaths, json_file);

        VortexMaker::RefreshEnvironmentProjectsPools();
        VortexMaker::RefreshEnvironmentProjects();
      }
      ImGui::EndPopup();
    }
  }
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
  std::make_shared<Cherry::ImageTextButtonSimple>("import_btn", "Import a project"); import_project_button->SetScale(0.85f);
  import_project_button->SetInternalMarginX(10.0f);
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

    ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#45454545")), ImGui::Separator();
    ImGui::PopStyleColor();

    ImGui::PushItemWidth(200);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 6.0f));  // Largeur x Hauteur padding
    ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#353535FF"));

    // input_search->Render("_");
    CherryKit::InputString("", &v_SearchString);

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

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
  ImGui::Separator();
  if (ImGui::BeginMenu("Filters")) {
    if (ImGui::MenuItem("Build/Rebuild single parts")) {
      handleFilterBuildRebuild();
    }
    if (ImGui::MenuItem("Global build")) {
      handleGlobalBuild();
    }
    ImGui::EndMenu();
  }
}

void ProjectManager::createMenuItem() {
  if (ImGui::BeginMenu("Create a module")) {
    if (ImGui::MenuItem("Build/Rebuild single parts")) {
      handleCreateModule();
    }
    if (ImGui::MenuItem("Global build")) {
      handleGlobalBuild();
    }
    ImGui::EndMenu();
  }
}

void ProjectManager::searchMenuItem() {
  if (ImGui::BeginMenu("Search")) {
    if (ImGui::MenuItem("Build/Rebuild single parts")) {
      handleSearch();
    }
    if (ImGui::MenuItem("Global build")) {
      handleGlobalBuild();
    }
    ImGui::EndMenu();
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