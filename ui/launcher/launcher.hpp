#pragma once
#define CHERRY_V1
#include "../../lib/cherry/cherry.hpp"

#include "src/static/project_manager/project_manager.hpp"
#include "src/static/main_settings/main_settings.hpp"
#include "src/static/welcome/welcome.hpp"
#include "src/static/logs/logs.hpp"
#include "src/static/download_center/download_center.hpp"
#include "src/static/logic_content_manager/logic_content_manager.hpp"
#include "src/static/content_manager/content_manager.hpp"
#include "src/static/version_manager/version_manager.hpp"

#include <thread>
#include <memory>

using namespace VortexLauncher;

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
    //
    // Static app windows
    //

    // Welcome
    welcome_window = WelcomeWindow::Create("?loc:loc.window_names.welcome");
    Cherry::AddAppWindow(welcome_window->GetAppWindow());    
    
    // Project manager
    project_manager = ProjectManager::Create("?loc:loc.window_names.project_manager");
    Cherry::AddAppWindow(project_manager->GetAppWindow());

    // Download center (change to Vortex Lab integratio)
    download_center = DownloadCenter::Create("Download Center");
    download_center->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(download_center->GetAppWindow());

    // Main settings
    system_settings = MainSettings::Create("?loc:loc.window_names.settings");
    system_settings->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(system_settings->GetAppWindow());

    // Logs window
    logs_window = LauncherLogUtility::Create("?loc:loc.window_names.logs_utility");
    logs_window->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(logs_window->GetAppWindow());

    // Logic content manager
    logic_content_manager = LogicContentManager::Create("?loc:loc.window_names.logic_content_manager");
    logic_content_manager->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(logic_content_manager->GetAppWindow());

    // Content manager
    content_manager = ContentManager::Create("?loc:loc.window_names.content_manager");
    content_manager->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(content_manager->GetAppWindow());

    // Version manager
    version_manager = VersionManagerAppWindow::Create("?loc:loc.window_names.version_manager");
    version_manager->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(version_manager->GetAppWindow());
 
 
 
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

  void SetLogicContentManager(const bool &visibility)
  {
    logic_content_manager->GetAppWindow()->SetVisibility(visibility);
  }
  void SetContentManager(const bool &visibility)
  {
    content_manager->GetAppWindow()->SetVisibility(visibility);
  }
  void SetVersionManager(const bool &visibility)
  {
    version_manager->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetVersionManager()
  {
    return version_manager->GetAppWindow()->m_Visible;
  }

  bool GetContentManager()
  {
    return content_manager->GetAppWindow()->m_Visible;
  }

  bool GetLogicContentManager()
  {
    return logic_content_manager->GetAppWindow()->m_Visible;
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
  std::shared_ptr<MainSettings> system_settings;
  std::shared_ptr<ProjectManager> project_manager;
  std::shared_ptr<DownloadCenter> download_center;
  std::shared_ptr<WelcomeWindow> welcome_window;
  std::shared_ptr<LogicContentManager> logic_content_manager;
  std::shared_ptr<VersionManagerAppWindow> version_manager;
  std::shared_ptr<ContentManager> content_manager;
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

 if (ImGui::BeginMenu("Vortex"))
                            {

                               ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("General");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);


                              if (ImGui::MenuItem("Update Vortex Launcher",  "", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icon.png")), c_Launcher->GetDownloadCenterVisibility()))
                              {
                              }


                               ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Vortex Labs");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);


                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.show_download_utility").c_str(), Cherry::GetLocale("loc.menubar.show_main_settings_brief").c_str(), Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icon.png")), c_Launcher->GetDownloadCenterVisibility()))
                              {
                                c_Launcher->SetDownloadCenterVisibility(!c_Launcher->GetDownloadCenterVisibility());
                              }

                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Manage");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.manage_modulesplugins").c_str(), "",  Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icon.png")), c_Launcher->GetLogicContentManager()))
                              {
                                c_Launcher->SetLogicContentManager(!c_Launcher->GetLogicContentManager());
                              }

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.manage_contents").c_str(),  "", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icon.png")), c_Launcher->GetContentManager()))
                              {
                                c_Launcher->SetContentManager(!c_Launcher->GetContentManager());
                              }

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.manage_vortex").c_str(),  "", c_Launcher->GetVersionManager()))
                              {
                                c_Launcher->SetVersionManager(!c_Launcher->GetVersionManager());
                              }

                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Other");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.about_vortex").c_str(), "", c_Launcher->GetLogsVisibility()))
                              {
                              }

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.contributors").c_str(),  "", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icon.png")), c_Launcher->GetDownloadCenterVisibility()))
                              {
                              }

                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.documentation").c_str(),  "", c_Launcher->GetMainSettingsVisibility()))
                              {
                              }


                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Options"))
                            {
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Utilities");
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

                              if (ImGui::MenuItem("Sessions utility", Cherry::GetLocale("loc.menubar.show_logs_utility_brief").c_str(), c_Launcher->GetLogsVisibility()))
                              {
                              }

                              if (ImGui::MenuItem("Last crash", Cherry::GetLocale("loc.menubar.show_logs_utility_brief").c_str(), c_Launcher->GetLogsVisibility()))
                              {
                              }
                              

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


                              if (ImGui::MenuItem(Cherry::GetLocale("loc.menubar.show_main_settings").c_str(), Cherry::GetLocale("loc.menubar.show_main_settings_brief").c_str(), c_Launcher->GetMainSettingsVisibility()))
                              {
                                c_Launcher->SetMainSettingsVisibility(!c_Launcher->GetMainSettingsVisibility());
                              }

    if (ImGui::BeginMenu("Switch language"))
    {

                              if (ImGui::MenuItem("English", "Switch language to english (en)", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/flags/us.png")), c_Launcher->GetMainSettingsVisibility()))
                              {
  app->SetLocale("en");
                              }
                              if (ImGui::MenuItem("Français", "Changer la langue pour le Français (fr)", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/flags/fr.png")), c_Launcher->GetMainSettingsVisibility()))
                              {
  app->SetLocale("fr");
                              }
                              if (ImGui::MenuItem("Español", "Cambiar el idioma a español", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/flags/es.png")), c_Launcher->GetMainSettingsVisibility()))
                              {
  app->SetLocale("en");
                              }
                              if (ImGui::MenuItem("Português", "Alterar o idioma para Português", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/flags/po.png")), c_Launcher->GetMainSettingsVisibility()))
                              {
  app->SetLocale("en");
                              }
                              if (ImGui::MenuItem("Deutsch", "Ändern Sie die Sprache auf Deutsch", Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/flags/po.png")), c_Launcher->GetMainSettingsVisibility()))
                              {
  app->SetLocale("ge");
                              }

      ImGui::EndMenu();
    }

                          

                              ImGui::EndMenu();
                            }

                            ImGui::PopStyleVar();  
                            ImGui::PopStyleColor(2); });

  c_Launcher = std::make_shared<Launcher>();
  return app;
}