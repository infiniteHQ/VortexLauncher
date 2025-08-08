#include "system_settings.hpp"

static std::string current_section = "mm";  // mm / pm tm cv
static std::shared_ptr<ModuleInterface> selected_module;
static std::shared_ptr<TemplateInterface> selected_template;
static std::string default_project_avatar = "/usr/local/include/Vortex/1.1/imgs/base_vortex.png";
static std::string _parent;

static bool open_module_deletion_modal = false;
static bool open_template_deletion_modal = false;
static bool open_import_module = false;
static bool open_import_all_module = false;
static bool open_import_all_templates = false;

static std::shared_ptr<ModuleInterface> single_module_to_add;

static std::vector<std::string> available_versions;

bool TestVortexExecutable(const std::string &path) {
  std::array<char, 128> buffer;
  std::string result;
  std::string command = path + " -test";

  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
  if (!pipe) {
    std::cerr << "popen() failed!" << std::endl;
    return false;
  }

  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  int return_code = pclose(pipe.release());

  return (result.find("ok") != std::string::npos) && (return_code == 0);
}

void RegisterAvailableVersions() {
  const std::string base_path = "/usr/local/bin/Vortex";
  available_versions.clear();
  for (const auto &entry : std::filesystem::directory_iterator(base_path)) {
    if (entry.is_directory()) {
      std::string version = entry.path().filename().string();
      std::string vortex_executable = entry.path().string() + "/vortex";

      if (TestVortexExecutable(vortex_executable)) {
        available_versions.push_back(version);
      }
    }
  }
}

enum class ProjectStates {
  SAME_NAME_ALREADY_EXIST,
  NOT_INSTALLED,
};
enum class ModulesStates {
  ALREADY_INSTALLED_WITH_CORRECT_VERSION,
  ALREADY_INSTALLED_WITH_ANOTHER_VERSION,
  NOT_INSTALLED,
};

enum class TemplatesStates {
  SAME_NAME_DETECTED,
  NOT_INSTALLED,
};

static std::unordered_map<std::string, std::pair<ModulesStates, std::shared_ptr<ModuleInterface>>> all_modules_to_add;
static std::vector<std::pair<TemplatesStates, std::shared_ptr<TemplateInterface>>> all_templates_to_add;

static void CheckAllTemplatesStates(std::vector<std::shared_ptr<TemplateInterface>> finded_templates) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  all_templates_to_add.clear();

  for (auto template_ : finded_templates) {
    bool resolved = false;

    for (auto sys_modules : ctx->IO.sys_templates) {
      if (resolved) {
        continue;
      }

      if (sys_modules->m_name == template_->m_name) {
        // Already installed
        std::pair<TemplatesStates, std::shared_ptr<TemplateInterface>> module_to_add;
        module_to_add.second = template_;
        module_to_add.first = TemplatesStates::SAME_NAME_DETECTED;
        all_templates_to_add.push_back(module_to_add);
        resolved = true;
      }
    }

    if (!resolved) {
      // Already installed, but not this version
      std::pair<TemplatesStates, std::shared_ptr<TemplateInterface>> module_to_add;
      module_to_add.second = template_;
      module_to_add.first = TemplatesStates::NOT_INSTALLED;
      all_templates_to_add.push_back(module_to_add);
      resolved = true;
    }
  }
}

static void InsertModule(std::pair<ModulesStates, std::shared_ptr<ModuleInterface>> module_to_add) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  std::string tag = module_to_add.second->m_name + module_to_add.second->m_version;

  bool finded = false;

  for (auto sys_modules : ctx->IO.sys_em) {
    if (finded)
      continue;

    if (sys_modules->m_name == module_to_add.second->m_name && sys_modules->m_version == module_to_add.second->m_version) {
      module_to_add.first = ModulesStates::ALREADY_INSTALLED_WITH_CORRECT_VERSION;
      finded = true;
    }

    if (sys_modules->m_name == module_to_add.second->m_name && !finded) {
      module_to_add.first = ModulesStates::ALREADY_INSTALLED_WITH_ANOTHER_VERSION;
    }
  }

  for (auto it = all_modules_to_add.begin(); it != all_modules_to_add.end(); it++) {
    if (it->second.second->m_name == module_to_add.second->m_name &&
        it->second.second->m_version == module_to_add.second->m_version) {
      it->second.first = ModulesStates::ALREADY_INSTALLED_WITH_CORRECT_VERSION;
    }
  }

  all_modules_to_add.insert({ tag, module_to_add });
}

static void CheckAllModulesStates(std::vector<std::shared_ptr<ModuleInterface>> finded_modules) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  all_modules_to_add.clear();

  for (auto module_ : finded_modules) {
    std::pair<ModulesStates, std::shared_ptr<ModuleInterface>> module_to_add;
    module_to_add.second = module_;
    module_to_add.first = ModulesStates::NOT_INSTALLED;
    InsertModule(module_to_add);
  }
}

static void MyButton(const std::string &name, int w, int h) {
  ImVec2 squareSize(w, h);
  ImVec2 totalSize(squareSize.x, squareSize.y + 5);
  ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

  std::string button_id = name + "squareButtonWithText";
  if (CherryGUI::InvisibleButton(button_id.c_str(), totalSize)) {
  }

  if (CherryGUI::IsItemHovered()) {
    CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  ImDrawList *drawList = CherryGUI::GetWindowDrawList();

  // addTexture(name, default_project_avatar);
  // getTexture(name, drawList, cursorPos, squareSize);

  ImVec2 smallRectSize(40, 20);
  ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

  drawList->AddRectFilled(
      smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));

  float windowVisibleX2 = CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
  if (cursorPos.x + totalSize.x < windowVisibleX2)
    CherryGUI::SameLine();
}

SystemSettings::SystemSettings(const std::string &name) {
  this->ctx = VortexMaker::GetCurrentContext();

  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"));
  m_AppWindow->SetDefaultBehavior(Cherry::DefaultAppWindowBehaviors::DefaultDocking, "full");

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetVisibility(false);
  m_AppWindow->SetCloseCallback([this]() { m_AppWindow->SetVisibility(false); });

  m_AppWindow->SetInternalPaddingY(8.0f);

  m_AppWindow->SetLeftMenubarCallback([this]() { this->menubar(); });

  this->Refresh();

  RegisterAvailableVersions();
}

bool MyParamBanner(const std::string &path) {
  bool pressed = false;
  // addTexture(path, path);
  ImVec2 squareSize(300, 51);

  ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

  std::string id = "## " + path;
  if (CherryGUI::InvisibleButton(id.c_str(), squareSize)) {
    pressed = true;
  }

  if (CherryGUI::IsItemHovered()) {
    CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  ImDrawList *drawList = CherryGUI::GetWindowDrawList();

  // getTexture(path, drawList, cursorPos, squareSize);

  ImVec2 smallRectSize(40, 20);
  ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);
  return pressed;
}

void SystemSettings::Render() {
  float columnWidths[3] = { 100.0f, 250.0f, 150.0f };

  ImVec2 windowSize = CherryGUI::GetWindowSize();

  float availableWidth = windowSize.x - CherryGUI::GetStyle().ItemSpacing.x * 2;

  float totalColumnWidths = 0.0f;
  for (int i = 0; i < 3; ++i)
    totalColumnWidths += columnWidths[i];

  float columnProportions[3];
  for (int i = 0; i < 3; ++i)
    columnProportions[i] = columnWidths[i] / totalColumnWidths;

  CherryGUI::Columns(3, nullptr, false);

  ImVec4 columnColors[3] = { ImVec4(1.0f, 0.0f, 0.0f, 0.0f),
                             ImVec4(0.0f, 0.0f, 1.0f, 0.0f),
                             ImVec4(0.0f, 1.0f, 0.0f, 0.0f) };

  CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
  CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
  for (int i = 0; i < 3; ++i) {
    float columnWidth = availableWidth * columnProportions[i];
    CherryGUI::SetColumnWidth(i, columnWidth);
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, columnColors[i]);
    CherryGUI::BeginChild(CherryGUI::GetID((void *)(intptr_t)i), ImVec2(columnWidth, 0), true);
    if (i == 0) {
      if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_modules_managment.png")) {
        current_section = "mm";
      }

      if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_plugins_managment.png")) {
        current_section = "pm";
      }

      if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_templates_managment.png")) {
        current_section = "tm";
      }

      if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_core_versions.png")) {
        current_section = "cv";
      }
    } else if (i == 1) {
      if (current_section == "mm") {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

        if (CherryGUI::BeginTable("tablhjke_", 6, flags)) {
          CherryGUI::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();

          CherryGUI::PopStyleVar(4);

          for (int row = 0; row < ctx->IO.sys_em.size(); row++) {
            static std::pair<char[128], char[128]> newItem;
            static char label[128];

            CherryGUI::TableNextRow();
            for (int column = 0; column < 6; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                if (CherryKit::ButtonImageText("create_project_button", Cherry::GetPath("resources/base/undefined"))
                        .GetData("isClicked") == "true") {
                  selected_module = ctx->IO.sys_em[row];
                }
              } else if (column == 1) {
                CherryGUI::Text(ctx->IO.sys_em[row]->m_proper_name.c_str());
              } else if (column == 2) {
                CherryGUI::Text(ctx->IO.sys_em[row]->m_name.c_str());
              } else if (column == 3) {
                CherryGUI::Text(ctx->IO.sys_em[row]->m_version.c_str());
              } else if (column == 4) {
                CherryGUI::Text(ctx->IO.sys_em[row]->m_path.c_str());
              } else if (column == 5) {
                CherryGUI::Text(ctx->IO.sys_em[row]->m_type.c_str());
              }
            }
          }
          CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
          CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
          CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
          CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

          CherryGUI::EndTable();
        }
      } else if (current_section == "tm") {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

        if (CherryGUI::BeginTable("tablhjke_", 5, flags)) {
          CherryGUI::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();

          CherryGUI::PopStyleVar(4);

          for (int row = 0; row < ctx->IO.sys_templates.size(); row++) {
            static std::pair<char[128], char[128]> newItem;
            static char label[128];

            CherryGUI::TableNextRow();
            for (int column = 0; column < 5; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                if (CherryKit::ButtonImageText("delete_button", Cherry::GetPath("resources/base/undefined"))
                        .GetData("isClicked") == "true") {
                  selected_template = ctx->IO.sys_templates[row];
                }
              } else if (column == 1) {
                CherryGUI::Text(ctx->IO.sys_templates[row]->m_proper_name.c_str());
              } else if (column == 2) {
                CherryGUI::Text(ctx->IO.sys_templates[row]->m_name.c_str());
              } else if (column == 3) {
                CherryGUI::Text(ctx->IO.sys_templates[row]->m_path.c_str());
              } else if (column == 4) {
                CherryGUI::Text(ctx->IO.sys_templates[row]->m_type.c_str());
              }
            }
          }
          CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
          CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
          CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
          CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

          CherryGUI::EndTable();
        }
      } else if (current_section == "cv") {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

        if (CherryGUI::BeginTable("tablhjke_", 2, flags)) {
          CherryGUI::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Prop", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();

          CherryGUI::PopStyleVar(4);

          for (int row = 0; row < available_versions.size(); row++) {
            CherryGUI::TableNextRow();
            for (int column = 0; column < 1; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                std::string label = "Vortex version " + available_versions[row];
                CherryGUI::Text(label.c_str());
              } else if (column == 1) {
                CherryGUI::Text(available_versions[row].c_str());
              }
            }
          }
          CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
          CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
          CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
          CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

          CherryGUI::EndTable();
        }
      }
    } else if (i == 2) {
      if (current_section == "mm") {
        if (!selected_module) {
          CherryGUI::Text("Select a module");
        } else {
          ImVec2 windowSize = CherryGUI::GetWindowSize();
          ImVec2 contentSize = CherryGUI::GetContentRegionAvail();
          ImVec2 buttonSize = ImVec2(100, 30);
          ImVec2 bitButtonSize = ImVec2(150, 30);
          {
            CherryGUI::BeginChild("LOGO_", ImVec2(70, 70), true);

            MyButton("tezt", 60, 60);

            CherryGUI::EndChild();
            CherryGUI::SameLine();
          }

          {
            CherryGUI::BeginChild("TEXT_", ImVec2(0, 68), true);
            float oldsize = CherryGUI::GetFont()->Scale;
            CherryGUI::GetFont()->Scale *= 1.3;
            CherryGUI::PushFont(CherryGUI::GetFont());

            CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_module->m_proper_name.c_str());

            CherryGUI::GetFont()->Scale = oldsize;
            CherryGUI::PopFont();
            CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_module->m_name.c_str());
            CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_module->m_version.c_str());
            CherryGUI::EndChild();
          }

          float ysection = windowSize.y - 280;
          CherryGUI::SetCursorPos(ImVec2(CherryGUI::GetCursorPosX(), ysection));

          float firstButtonPosX =
              windowSize.x - buttonSize.x - bitButtonSize.y - 75 * 2 - CherryGUI::GetStyle().ItemSpacing.x * 3;

          float buttonPosY = windowSize.y - buttonSize.y - CherryGUI::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

          CherryGUI::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

          if (CherryGUI::Button("Rebuild", buttonSize)) {
          }

          CherryGUI::SameLine();

          CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.2f, 0.8f));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.2f, 1.8f));
          if (CherryGUI::Button("Delete module", bitButtonSize)) {
            open_module_deletion_modal = true;
          }
          CherryGUI::PopStyleColor(3);
        }
      }

      if (current_section == "tm") {
        if (!selected_template) {
          CherryGUI::Text("Select a template");
        } else {
          ImVec2 windowSize = CherryGUI::GetWindowSize();
          ImVec2 contentSize = CherryGUI::GetContentRegionAvail();
          ImVec2 buttonSize = ImVec2(100, 30);
          ImVec2 bitButtonSize = ImVec2(150, 30);
          {
            CherryGUI::BeginChild("LOGO_", ImVec2(70, 70), true);

            MyButton("tezt", 60, 60);

            CherryGUI::EndChild();
            CherryGUI::SameLine();
          }

          {
            CherryGUI::BeginChild("TEXT_", ImVec2(0, 68), true);
            float oldsize = CherryGUI::GetFont()->Scale;
            CherryGUI::GetFont()->Scale *= 1.3;
            CherryGUI::PushFont(CherryGUI::GetFont());

            CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_template->m_proper_name.c_str());

            CherryGUI::GetFont()->Scale = oldsize;
            CherryGUI::PopFont();
            CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_template->m_name.c_str());
            CherryGUI::EndChild();
          }

          float ysection = windowSize.y - 280;
          CherryGUI::SetCursorPos(ImVec2(CherryGUI::GetCursorPosX(), ysection));

          float firstButtonPosX =
              windowSize.x - buttonSize.x - bitButtonSize.y - 75 * 2 - CherryGUI::GetStyle().ItemSpacing.x * 3;

          float buttonPosY = windowSize.y - buttonSize.y - CherryGUI::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

          CherryGUI::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

          if (CherryGUI::Button("Rebuild", buttonSize)) {
          }

          CherryGUI::SameLine();

          CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.2f, 0.8f));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.2f, 1.8f));
          if (CherryGUI::Button("Delete template", bitButtonSize)) {
            open_template_deletion_modal = true;
          }
          CherryGUI::PopStyleColor(3);
        }
      }
    }
    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();

    if (i < 2)
      CherryGUI::NextColumn();
  }
  CherryGUI::PopStyleVar(4);
}

std::shared_ptr<Cherry::AppWindow> &SystemSettings::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<SystemSettings> SystemSettings::Create(const std::string &name) {
  auto instance = std::shared_ptr<SystemSettings>(new SystemSettings(name));
  instance->SetupRenderCallback();
  return instance;
}

void SystemSettings::SetupRenderCallback() {
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

void SystemSettings::addModuleModal() {
}

void SystemSettings::mainButtonsMenuItem() {
}

void SystemSettings::filterMenuItem() {
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

void SystemSettings::createMenuItem() {
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

void SystemSettings::searchMenuItem() {
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

void SystemSettings::menubar() {
  if (open_import_all_templates) {
    static ImGuiTableFlags window_flags = ImGuiWindowFlags_MenuBar;
    if (CherryGUI::BeginPopupModal("Search templates in folder", NULL, window_flags)) {
      static char path_input_template_all[512];

      if (CherryGUI::BeginMenuBar()) {
        CherryGUI::Text("Please chose a folder");

        if (CherryKit::ButtonImageText("refresh_project_button", Cherry::GetPath("resources/base/undefined"))
                .GetData("isClicked") == "true") {
          //
        }

        CherryGUI::InputText("###Path", path_input_template_all, IM_ARRAYSIZE(path_input_template_all));
        std::string label = "Find###templates";
        if (CherryGUI::Button(label.c_str())) {
          CheckAllTemplatesStates(VortexMaker::FindTemplatesInDirectory(path_input_template_all));
        }
        CherryGUI::EndMenuBar();
      }

      if (!all_templates_to_add.empty()) {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

        if (CherryGUI::BeginTable("ModulesToAddTable", 4, flags)) {
          CherryGUI::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();

          for (int row = 0; row < all_templates_to_add.size(); row++) {
            static std::pair<char[128], char[128]> newItem;
            static char label[128];

            CherryGUI::TableNextRow();
            for (int column = 0; column < 3; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                if (all_templates_to_add[row].first == TemplatesStates::SAME_NAME_DETECTED) {
                  CherryGUI::Text("Handle versions !!! But already here");
                } else if (all_templates_to_add[row].first == TemplatesStates::NOT_INSTALLED) {
                  std::string label = "Install###" + row;
                  if (CherryGUI::Button(label.c_str())) {
                                      /*
                                        VxContext *ctx = VortexMaker::GetCurrentContext();
                                        VortexMaker::InstallTemplateOnSystem(all_templates_to_add[row].second->m_path);
                                        VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);
                                        CheckAllTemplatesStates(VortexMaker::FindTemplatesInDirectory(path_input_template_all));
                                   
                                   */ }
                }
              } else if (column == 1) {
                CherryGUI::Text(all_templates_to_add[row].second->m_proper_name.c_str());
              } else if (column == 2) {
                CherryGUI::Text(all_templates_to_add[row].second->m_name.c_str());
              } else if (column == 3) {
                CherryGUI::Text(all_templates_to_add[row].second->m_version.c_str());
              }
            }
          }

          CherryGUI::EndTable();
        }
      }

      // static int unused_i = 0;
      // CherryGUI::Combo("Combo", &unused_i, "Delete\0Delete harder\0");
      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 buttonSize = ImVec2(100, 30);
      ImVec2 bitButtonSize = ImVec2(150, 30);
      float ysection = windowSize.y - 280;
      CherryGUI::SetCursorPos(ImVec2(CherryGUI::GetCursorPosX(), ysection));

      float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - CherryGUI::GetStyle().ItemSpacing.x * 3;

      float buttonPosY = windowSize.y - buttonSize.y - CherryGUI::GetStyle().ItemSpacing.y * 2 - 10;

      CherryGUI::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

      if (CherryGUI::Button("Done", ImVec2(120, 0))) {
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::SetItemDefaultFocus();
      CherryGUI::SameLine();
      if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
        open_import_all_templates = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::EndPopup();
    }
  }

  if (open_template_deletion_modal) {
    CherryGUI::SetNextWindowSize(ImVec2(300, 200));

    static ImGuiTableFlags window_flags = ImGuiWindowFlags_NoResize;
    if (CherryGUI::BeginPopupModal("Delete a system template", NULL, window_flags)) {
      // Set the size of the modal to 200x75 pixels the first time it is created

      // 3 text inputs
      static char path_input_all[512];
      // inputs widget
      CherryGUI::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
      CherryGUI::SetItemDefaultFocus();

      if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
        open_template_deletion_modal = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::SameLine();
      CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
      if (CherryGUI::Button("Delete", ImVec2(120, 0))) {
        // Delete
        VortexMaker::DeleteSystemTemplate(selected_template->m_name, selected_template->m_version);
        VortexMaker::LoadSystemTemplates(this->ctx->IO.sys_templates);

        open_template_deletion_modal = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::PopStyleColor(3);
      CherryGUI::EndPopup();
    }
  }

  if (open_module_deletion_modal) {
    CherryGUI::SetNextWindowSize(ImVec2(300, 200));

    static ImGuiTableFlags window_flags = ImGuiWindowFlags_NoResize;
    if (CherryGUI::BeginPopupModal("Delete a system module", NULL, window_flags)) {
      // Set the size of the modal to 200x75 pixels the first time it is created

      // 3 text inputs
      static char path_input_all[512];
      // inputs widget
      CherryGUI::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
      CherryGUI::SetItemDefaultFocus();

      if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
        open_module_deletion_modal = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::SameLine();
      CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
      if (CherryGUI::Button("Delete", ImVec2(120, 0))) {
        // Delete
        VortexMaker::DeleteSystemModule(selected_module->m_name, selected_module->m_version);
        VortexMaker::LoadSystemModules(this->ctx->IO.sys_em);

        open_module_deletion_modal = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::PopStyleColor(3);
      CherryGUI::EndPopup();
    }
  }

  if (open_import_all_module) {
    static ImGuiTableFlags window_flags = ImGuiWindowFlags_MenuBar;
    static bool first_time = true;

    if (first_time) {
      CherryGUI::SetNextWindowSize(ImVec2(820, 420));
    }

    if (CherryGUI::BeginPopupModal("Search modules in folder", NULL, window_flags)) {
      if (first_time) {
        first_time = false;
      }

      static char path_input_all[512];

      if (CherryGUI::BeginMenuBar()) {
        CherryGUI::Text("Please chose a folder");

        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/base/undefined")).GetData("isClicked") == "true") {
        }

        CherryGUI::InputText("###Path", path_input_all, IM_ARRAYSIZE(path_input_all));
        std::string label = "Find###templates";
        if (CherryGUI::Button(label.c_str())) {
          CheckAllModulesStates(VortexMaker::FindModulesInDirectory(path_input_all));
        }
        CherryGUI::EndMenuBar();
      }

      if (!all_modules_to_add.empty()) {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

        if (CherryGUI::BeginTable("ModulesToAddTable", 4, flags)) {
          CherryGUI::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();

          for (auto it = all_modules_to_add.begin(); it != all_modules_to_add.end(); it++) {
            static std::pair<char[128], char[128]> newItem;
            static char label[128];

            CherryGUI::TableNextRow();
            for (int column = 0; column < 4; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                if (it->second.first == ModulesStates::ALREADY_INSTALLED_WITH_CORRECT_VERSION) {
                  CherryGUI::Text("Already installed");
                } else if (it->second.first == ModulesStates::ALREADY_INSTALLED_WITH_ANOTHER_VERSION) {
                  std::string label = "Install v" + it->second.second->m_version;
                  if (CherryGUI::Button(label.c_str())) {
                    VxContext *ctx = VortexMaker::GetCurrentContext();
                    // VortexMaker::InstallModuleToSystem(it->second.second->m_path);
                    VortexMaker::LoadSystemModules(ctx->IO.sys_em);
                    CheckAllModulesStates(VortexMaker::FindModulesInDirectory(path_input_all));
                  }
                } else if (it->second.first == ModulesStates::NOT_INSTALLED) {
                  if (CherryGUI::Button("Install")) {
                    VxContext *ctx = VortexMaker::GetCurrentContext();
                    // VortexMaker::InstallModuleToSystem(it->second.second->m_path);
                    VortexMaker::LoadSystemModules(ctx->IO.sys_em);
                    CheckAllModulesStates(VortexMaker::FindModulesInDirectory(path_input_all));
                  }
                }
              } else if (column == 1) {
                CherryGUI::Text(it->second.second->m_proper_name.c_str());
              } else if (column == 2) {
                CherryGUI::Text(it->second.second->m_name.c_str());
              } else if (column == 3) {
                CherryGUI::Text(it->second.second->m_version.c_str());
              }
            }
          }

          CherryGUI::EndTable();
        }
      }

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 buttonSize = ImVec2(100, 30);
      ImVec2 bitButtonSize = ImVec2(150, 30);
      float ysection = windowSize.y - 280;
      CherryGUI::SetCursorPos(ImVec2(CherryGUI::GetCursorPosX(), ysection));

      float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - CherryGUI::GetStyle().ItemSpacing.x * 3;

      float buttonPosY = windowSize.y - buttonSize.y - CherryGUI::GetStyle().ItemSpacing.y * 2 - 10;

      CherryGUI::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

      if (CherryGUI::Button("Done", ImVec2(120, 0))) {
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::SetItemDefaultFocus();
      CherryGUI::SameLine();
      if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
        open_import_all_module = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::EndPopup();
    }
  }

  if (open_import_module) {
    if (CherryGUI::BeginPopupModal("Import a module", NULL, NULL)) {
      // Set the size of the modal to 200x75 pixels the first time it is created

      // 3 text inputs
      static char path_input[512];
      // inputs widget
      CherryGUI::TextWrapped(
          "Please provide the path of your module, you need to include the module folder like "
          "\"/path/to/your/module/ModuleFolder\"");
      CherryGUI::InputText("Module path", path_input, IM_ARRAYSIZE(path_input));
      if (CherryGUI::Button("Find")) {
        single_module_to_add = VortexMaker::FindModuleInDirectory(path_input);
      }

      if (single_module_to_add) {
        CherryGUI::Text(single_module_to_add->m_name.c_str());
      }

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 buttonSize = ImVec2(100, 30);
      ImVec2 bitButtonSize = ImVec2(150, 30);
      float ysection = windowSize.y - 280;
      CherryGUI::SetCursorPos(ImVec2(CherryGUI::GetCursorPosX(), ysection));

      float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - CherryGUI::GetStyle().ItemSpacing.x * 3;

      float buttonPosY = windowSize.y - buttonSize.y - CherryGUI::GetStyle().ItemSpacing.y * 2 - 10;

      CherryGUI::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

      if (CherryGUI::Button("Done", ImVec2(120, 0))) {
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::SetItemDefaultFocus();
      CherryGUI::SameLine();
      if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
        open_import_module = false;
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::EndPopup();
    }
  }

  if (open_import_module)
    CherryGUI::OpenPopup("Import a module");

  if (open_import_all_module)
    CherryGUI::OpenPopup("Search modules in folder");

  if (open_import_all_templates)
    CherryGUI::OpenPopup("Search templates in folder");

  if (open_module_deletion_modal)
    CherryGUI::OpenPopup("Delete a system module");

  if (open_template_deletion_modal)
    CherryGUI::OpenPopup("Delete a system template");

  if (CherryGUI::BeginMenuBar()) {
    if (current_section == "mm") {
      if (CherryKit::ButtonImageText("Import a module", Cherry::GetPath("resources/base/undefined")).GetData("isClicked") ==
          "true") {
        open_import_module = true;
      }

      if (CherryKit::ButtonImageText("Search modules in a folder", Cherry::GetPath("resources/base/undefined"))
              .GetData("isClicked") == "true") {
        open_import_all_module = true;
      }
    }
    if (current_section == "tm") {
      if (CherryKit::ButtonImageText("Search modules in a folder", Cherry::GetPath("resources/base/undefined"))
              .GetData("isClicked") == "true") {
        open_import_all_templates = true;
      }
    }
    if (current_section == "pm") {
      if (CherryKit::ButtonImageText("Install a plugin", Cherry::GetPath("resources/base/undefined")).GetData("isClicked") ==
          "true") {
        //
      }
    }
    if (current_section == "cv") {
      if (CherryKit::ButtonImageText("Refresh versions", Cherry::GetPath("resources/base/undefined")).GetData("isClicked") ==
          "true") {
        RegisterAvailableVersions();
      }

      if (CherryKit::ButtonImageText("Download a newer version", Cherry::GetPath("resources/base/undefined"))
              .GetData("isClicked") == "true") {
        //
      }

      if (CherryKit::ButtonImageText("Upgrade a project", Cherry::GetPath("resources/base/undefined"))
              .GetData("isClicked") == "true") {
        //
      }

      if (CherryKit::ButtonImageText("Upgrade a module", Cherry::GetPath("resources/base/undefined")).GetData("isClicked") ==
          "true") {
        //
      }
    }
    CherryGUI::EndMenuBar();
  }
}

void SystemSettings::Refresh() {
  project_templates.clear();
  for (auto tem : this->ctx->IO.sys_templates) {
    if (tem->m_type == "project") {
      project_templates.push_back(tem);
    }
  }
};