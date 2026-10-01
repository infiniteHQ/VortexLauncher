
#include "logs.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;

namespace VortexLauncher {

  LauncherLogUtility::LauncherLogUtility(const std::string& name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);

    m_AppWindow->SetInternalPaddingX(10.0f);
    m_AppWindow->SetInternalPaddingY(10.0f);

    m_AppWindow->SetVisibility(true);
    m_AppWindow->SetCloseCallback([this]() { m_AppWindow->SetVisibility(false); });

    this->ctx = VortexMaker::GetCurrentContext();
  }

  std::shared_ptr<Cherry::AppWindow>& LauncherLogUtility::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<LauncherLogUtility> LauncherLogUtility::Create(const std::string& name) {
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
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    const float TEXT_BASE_WIDTH = CherryGUI::CalcTextSize("A").x;

    if (CherryGUI::BeginTable("3ways", 4, flags)) {
      CherryGUI::TableSetupColumn("Level", ImGuiTableColumnFlags_NoHide);
      CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
      CherryGUI::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
      CherryGUI::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
      CherryGUI::TableHeadersRow();

      for (const auto& log : ctx->registered_logs) {
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
    //
  }
}  // namespace VortexLauncher