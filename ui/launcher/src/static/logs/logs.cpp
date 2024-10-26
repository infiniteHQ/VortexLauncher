
#include "logs.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;

namespace VortexLauncher
{

    LauncherLogUtility::LauncherLogUtility(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        m_AppWindow->SetVisibility(true);
        m_AppWindow->SetCloseCallback([this]()
                                      { m_AppWindow->SetVisibility(false); });

        m_AppWindow->SetLeftMenubarCallback([this]()
                                            { menubar(); });

        this->ctx = VortexMaker::GetCurrentContext();
    }

    std::shared_ptr<Cherry::AppWindow> &LauncherLogUtility::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<LauncherLogUtility> LauncherLogUtility::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<LauncherLogUtility>(new LauncherLogUtility(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void LauncherLogUtility::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void LauncherLogUtility::Render()
    {
        Cherry::Application::GetCurrentRenderedWindow()->SetFavIcon(Cherry::GetPath("resources/imgs/icon_crash.png"));
        float oldsize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 1.3;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project contents of : ");
        ImGui::SameLine();
        // ImGui::Text(this->ctx->name.c_str());

        ImGui::GetFont()->Scale = oldsize;
        ImGui::PopFont();

        ImGui::Separator();

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

        if (ImGui::BeginTable("3ways", 4, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableHeadersRow();

            for (auto log : ctx->registered_logs)
            {
                if (log->m_level == spdlog::level::critical && !FatalFilter)
                    continue;
                if (log->m_level == spdlog::level::err && !ErrorFilter)
                    continue;
                if (log->m_level == spdlog::level::warn && !WarnFilter)
                    continue;
                if (log->m_level == spdlog::level::info && !InfoFilter)
                    continue;

                ImGui::TableNextRow();
                for (int i = 0; i <= 3; i++)
                {
                    ImGui::TableSetColumnIndex(i);
                    if (i == 0)
                    {
                        if (log->m_level == spdlog::level::critical)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
                        }
                        else if (log->m_level == spdlog::level::err)
                        {
                            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
                        }
                        else if (log->m_level == spdlog::level::warn)
                        {
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
                        }
                        else if (log->m_level == spdlog::level::info)
                        {
                            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
                        }
                    }
                    else if (i == 1)
                    {
                        ImGui::Text(log->m_timestamp.c_str());
                    }
                    else if (i == 2)
                    {
                        ImGui::Text(log->m_filter.c_str());
                    }
                    else if (i == 3)
                    {
                        ImGui::Text(log->m_message.c_str());
                    }
                }
            }
            ImGui::EndTable();
        }
    }

    void LauncherLogUtility::menubar()
    {
        static std::shared_ptr<Cherry::ImageTextButtonSimple> refresh_button = std::make_shared<Cherry::ImageTextButtonSimple>("logs_refresh_project_button", "Refresh");
        refresh_button->SetScale(0.85f);
        refresh_button->SetInternalMarginX(10.0f);
        refresh_button->SetLogoSize(15, 15);
        refresh_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));

        static std::shared_ptr<Cherry::ImageTextButtonSimple> add_button = std::make_shared<Cherry::ImageTextButtonSimple>("logs_create_project_button", "Add");
        add_button->SetScale(0.85f);
        add_button->SetInternalMarginX(10.0f);
        add_button->SetLogoSize(15, 15);
        add_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));

        if (refresh_button->Render())
        {
        }
        if (add_button->Render())
        {
            ImGui::OpenPopup("CreationMenu");
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Filters"))
        {
            ImGui::Checkbox("Show informations", &InfoFilter);
            ImGui::Checkbox("Show fatal errors", &FatalFilter);
            ImGui::Checkbox("Show errors", &ErrorFilter);
            ImGui::Checkbox("Show warnings", &WarnFilter);
            if (ImGui::MenuItem("Build/Rebuild single parts"))
            {
                // Behavior
            }
            if (ImGui::MenuItem("Global build"))
            {
                // Behavior
            }
            ImGui::EndMenu();
        }
    }
}