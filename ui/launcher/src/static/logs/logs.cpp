
#include "logs.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;

namespace VortexLauncher {

  LauncherLogUtility::LauncherLogUtility(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");

    m_AppWindow->SetInternalPaddingX(10.0f);
    m_AppWindow->SetInternalPaddingY(10.0f);

    m_AppWindow->SetVisibility(true);
    m_AppWindow->SetCloseCallback([this]() { m_AppWindow->SetVisibility(false); });

    m_AppWindow->SetLeftMenubarCallback([this]() { menubar(); });

    this->ctx = VortexMaker::GetCurrentContext();
  }

  std::shared_ptr<Cherry::AppWindow> &LauncherLogUtility::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<LauncherLogUtility> LauncherLogUtility::Create(const std::string &name) {
    auto instance = std::shared_ptr<LauncherLogUtility>(new LauncherLogUtility(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void LauncherLogUtility::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  void LauncherLogUtility::Render() {
    float oldsize = CherryGUI::GetFont()->Scale;
    CherryGUI::GetFont()->Scale *= 1.3;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project contents of : ");
    CherryGUI::SameLine();
    // CherryGUI::Text(this->ctx->name.c_str());

    CherryGUI::GetFont()->Scale = oldsize;
    CherryGUI::PopFont();

    CherryGUI::Separator();

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    const float TEXT_BASE_WIDTH = CherryGUI::CalcTextSize("A").x;

    if (CherryGUI::BeginTable("3ways", 4, flags)) {
      CherryGUI::TableSetupColumn("Level", ImGuiTableColumnFlags_NoHide);
      CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
      CherryGUI::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
      CherryGUI::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
      CherryGUI::TableHeadersRow();

      for (const auto &log : ctx->registered_logs) {
        if (log->m_level == VxLogLevel::critical && !FatalFilter)
          continue;
        if (log->m_level == VxLogLevel::err && !ErrorFilter)
          continue;
        if (log->m_level == VxLogLevel::warn && !WarnFilter)
          continue;
        if (log->m_level == VxLogLevel::info && !InfoFilter)
          continue;

        CherryGUI::TableNextRow();

        CherryGUI::TableSetColumnIndex(0);
        switch (log->m_level) {
          case VxLogLevel::critical: CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal"); break;
          case VxLogLevel::err: CherryGUI::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error"); break;
          case VxLogLevel::warn: CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning"); break;
          case VxLogLevel::info: CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information"); break;
        }

        CherryGUI::TableSetColumnIndex(1);
        CherryGUI::Text(log->m_timestamp.c_str());

        CherryGUI::TableSetColumnIndex(2);
        CherryGUI::Text(log->m_filter.c_str());

        CherryGUI::TableSetColumnIndex(3);
        CherryGUI::Text(log->m_message.c_str());
      }
      CherryGUI::EndTable();
    }
  }

  void LauncherLogUtility::menubar() {
    /*static std::shared_ptr<Cherry::ImageTextButtonSimple> refresh_button =
    std::make_shared<Cherry::ImageTextButtonSimple>("logs_refresh_project_button", "Refresh");
    refresh_button->SetScale(0.85f);
    refresh_button->SetInternalMarginX(10.0f);
    refresh_button->SetLogoSize(15, 15);
    refresh_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));*/

    /*static std::shared_ptr<Cherry::ImageTextButtonSimple> add_button =
    std::make_shared<Cherry::ImageTextButtonSimple>("logs_create_project_button", "Add"); add_button->SetScale(0.85f);
    add_button->SetInternalMarginX(10.0f);
    add_button->SetLogoSize(15, 15);
    add_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));*/

    if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"))
            .GetData("isClicked") == "true") {
      //
    }
    if (CherryKit::ButtonImageText("Add", Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")).GetData("isClicked") ==
        "true") {
      CherryGUI::OpenPopup("CreationMenu");
    }
    CherryGUI::Separator();
    if (CherryGUI::BeginMenu("Filters")) {
      CherryGUI::Checkbox("Show informations", &InfoFilter);
      CherryGUI::Checkbox("Show fatal errors", &FatalFilter);
      CherryGUI::Checkbox("Show errors", &ErrorFilter);
      CherryGUI::Checkbox("Show warnings", &WarnFilter);
      if (CherryGUI::MenuItem("Build/Rebuild single parts")) {
        // Behavior
      }
      if (CherryGUI::MenuItem("Global build")) {
        // Behavior
      }
      CherryGUI::EndMenu();
    }
  }
}  // namespace VortexLauncher