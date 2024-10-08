#pragma once
#define CHERRY_V1
#include "../../lib/cherry/cherry.hpp"

#include "src/static/project_manager/project_manager.hpp"
#include "src/static/main_settings/main_settings.hpp"
#include "src/static/welcome/welcome.hpp"
#include "src/static/logs/logs.hpp"
#include "src/static/download_center/download_center.hpp"

#include <thread>
#include <memory>

class Layer : public Cherry::Layer
{
public:
  Layer() {};
};

class Launcher
{
public:
  Launcher()
  {
    // Render static windows
    welcome_window = std::make_shared<Cherry::WelcomeWindow>("?loc:loc.window_names.welcome");
    welcome_window->RefreshRender(welcome_window);
    Cherry::AddWindow(welcome_window->GetAppWindow()); // Publish this window into the workspace

    download_center = std::make_shared<DownloadCenter>("Download Center");
    download_center->RefreshRender(download_center);
    download_center->GetAppWindow()->SetVisibility(false);
    Cherry::AddWindow(download_center->GetAppWindow()); // Publish this window into the workspace

    project_manager = std::make_shared<ProjectManager>();
    project_manager->RefreshRender(project_manager);
    Cherry::AddWindow(project_manager->GetAppWindow()); // Publish this window into the workspace

    system_settings = std::make_shared<Cherry::MainSettings>("?loc:loc.window_names.settings");
    system_settings->RefreshRender(system_settings);
    system_settings->GetAppWindow()->SetVisibility(false);
    Cherry::AddWindow(system_settings->GetAppWindow()); // Publish this window into the workspace

    logs_window = std::make_shared<LauncherLogUtility>("?loc:loc.window_names.logs_utility");
    logs_window->RefreshRender(logs_window);
    logs_window->GetAppWindow()->SetVisibility(false);
    Cherry::AddWindow(logs_window->GetAppWindow()); // Publish this window into the workspace
  };

  void SetLogsVisibility(const bool &visibility)
  {
    logs_window->GetAppWindow()->SetVisibility(visibility);
  }

  void SetMainSettingsVisibility(const bool &visibility)
  {
    system_settings->GetAppWindow()->SetVisibility(visibility);
  }

  void SetProjectManagerVisibility(const bool &visibility)
  {
    project_manager->GetAppWindow()->SetVisibility(visibility);
  }

  void SetWelcomeWindowVisibility(const bool &visibility)
  {
    welcome_window->GetAppWindow()->SetVisibility(visibility);
  }

  void SetDownloadCenterVisibility(const bool &visibility)
  {
    download_center->GetAppWindow()->SetVisibility(visibility);
    if (visibility)
    {
      Cherry::ApplicationSpecification spec;
      std::shared_ptr<Layer> layer = std::make_shared<Layer>();

      std::string name = "Download Center";
      spec.Name = name;
      spec.MinHeight = 500;
      spec.MinWidth = 500;
      spec.Height = 500;
      spec.Width = 950;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = true;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
      spec.UniqueAppWindowName = download_center->GetAppWindow()->m_Name;

      spec.DisableTitle = true;
      spec.WindowSaves = false;
      spec.IconPath = Cherry::GetPath("ressources/imgs/icon_update.png");
      download_center->GetAppWindow()->AttachOnNewWindow(spec);
    }

  }

  bool GetDownloadCenterVisibility()
  {
    return download_center->GetAppWindow()->m_Visible;
  }

  bool GetLogsVisibility()
  {
    return logs_window->GetAppWindow()->m_Visible;
  }

  bool GetMainSettingsVisibility()
  {
    return system_settings->GetAppWindow()->m_Visible;
  }

  bool GetProjectManagerVisibility()
  {
    return project_manager->GetAppWindow()->m_Visible;
  }

  bool GetWelcomeWindowVisibility()
  {
    return welcome_window->GetAppWindow()->m_Visible;
  }

private:
  std::shared_ptr<LauncherLogUtility> logs_window;
  std::shared_ptr<Cherry::MainSettings> system_settings;
  std::shared_ptr<ProjectManager> project_manager;
  std::shared_ptr<DownloadCenter> download_center;
  std::shared_ptr<Cherry::WelcomeWindow> welcome_window;
};

static std::shared_ptr<Launcher> c_Launcher;

Cherry::Application *Cherry::CreateApplication(int argc, char **argv)
{
  Cherry::ApplicationSpecification spec;
  std::shared_ptr<Layer> layer = std::make_shared<Layer>();

  std::string name = "Vortex Launcher";
  spec.Name = name;
  spec.MinHeight = 500;
  spec.MinWidth = 500;
  spec.Height = 600;
  spec.Width = 1200;
  spec.CustomTitlebar = true;
  spec.DisableWindowManagerTitleBar = true;
  spec.WindowOnlyClosable = true;
  spec.RenderMode = WindowRenderingMethod::DockingWindows;

  spec.DisableTitle = true;
  spec.WindowSaves = false;
  spec.IconPath = Cherry::GetPath("ressources/imgs/icon.png");

  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::GetPath("ressources/imgs/favicon.png"));
  app->AddFont("Consola", Cherry::GetPath("ressources/fonts/consola.ttf"), 17.0f);

  app->AddLocale("fr", Cherry::GetPath("ressources/locales/fr.json"));
  app->AddLocale("en", Cherry::GetPath("ressources/locales/en.json"));
  app->SetDefaultLocale("en");
  app->SetLocale("fr");

  app->PushLayer(layer);
  app->SetMenubarCallback([app, layer]()
                          {
                            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        
                            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
                            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
                            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);       

                            ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

                            ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

                            ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

                            static bool t;

                            if (ImGui::BeginMenu("Options"))
                            {
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Settings");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.show_logs_utility").c_str(), Cherry::GetLocale("loc.menubar.show_logs_utility_brief").c_str(), c_Launcher->GetLogsVisibility()))
                              {
                                c_Launcher->SetLogsVisibility(!c_Launcher->GetLogsVisibility());
                              }

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.show_download_utility").c_str(), Cherry::GetLocale("loc.menubar.show_main_settings_brief").c_str(), c_Launcher->GetDownloadCenterVisibility()))
                              {
                                c_Launcher->SetDownloadCenterVisibility(!c_Launcher->GetDownloadCenterVisibility());
                              }

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.show_main_settings").c_str(), Cherry::GetLocale("loc.menubar.show_main_settings_brief").c_str(), c_Launcher->GetMainSettingsVisibility()))
                              {
                                c_Launcher->SetMainSettingsVisibility(!c_Launcher->GetMainSettingsVisibility());
                              }

                              if(ImGui::Button("Set en"))
                              {
  app->SetLocale("en");
                              }

                              if(ImGui::Button("Set fr"))
                              {
  app->SetLocale("fr");
                              }

                              if(ImGui::Button("Set es"))
                              {
  app->SetLocale("es");
                              }
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem("Logs Utility", "Overview of all logs", &t))
                              {
                              }

                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
                              {
                              }

                              if (ImGui::MenuItem("Manage modules", "Manage modules loaded/registered", &t))
                              {
                              }

                              if (ImGui::MenuItem("Templates modules", "Create, add template in your project", &t))
                              {
                              }

                              ImGui::EndMenu();
                            }

                            ImGui::PopStyleVar();  
                            ImGui::PopStyleColor(2); });

  c_Launcher = std::make_shared<Launcher>();
  return app;
}