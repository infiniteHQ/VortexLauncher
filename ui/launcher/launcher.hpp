#pragma once

#ifndef VORTEXLAUNCHER_LAUNCHER_H
#define VORTEXLAUNCHER_LAUNCHER_H

#include <memory>
#include <thread>

#include "../../lib/cherry/cherry.hpp"
#include "src/static/about/about.hpp"
#include "src/static/content_manager/content_manager.hpp"
// #include "src/static/download_center/download_center.hpp"
#include "src/static/logic_content_manager/logic_content_manager.hpp"
#include "src/static/logs/logs.hpp"
#include "src/static/main_settings/main_settings.hpp"
// #include "src/static/project_manager/project_manager.hpp"
#include "src/static/version_manager/version_manager.hpp"
#include "src/static/welcome/welcome.hpp"

using namespace VortexLauncher;

class Layer : public Cherry::Layer {
 public:
  Layer() { };
};

class Launcher {
 public:
  Launcher() {
    /*if (!VortexMaker::GetCurrentContext()->blankproject_exist) {
      std::string cmd;

#ifdef _WIN32
      std::string path;
      for (auto template_path : VortexMaker::GetCurrentContext()->IO.sys_templates_pools) {
        path = template_path;
        break;
      }
      std::string source = Cherry::GetPath("resources/templates/blank_project");
      cmd = "xcopy \"" + source + "\" \"" + path + "\" /E /I /Y";
#else
      std::string path;
      for (auto template_path : VortexMaker::GetCurrentContext()->IO.sys_templates_pools) {
        path = template_path;
        break;
      }
      cmd = "cp -r " + Cherry::GetPath("resources/templates/blank_project") + " " + path;
#endif

      std::cout << "CPUY <" << cmd << std::endl;
      system(cmd.c_str());
    }*/

    //
    // Static app windows
    //

    // About
    about_window = AboutAppWindow::Create("?loc:loc.window_names.about");
    about_window->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(about_window->GetAppWindow());

    // Welcome
    welcome_window = WelcomeWindow::Create("?loc:loc.window_names.welcome");
    Cherry::AddAppWindow(welcome_window->GetAppWindow());
    /*welcome_window->m_CreateProjectCallback = [this]() {
      project_manager->m_ProjectCreation = true;
      CherryGUI::SetWindowFocus(project_manager->GetAppWindow()->m_IdName.c_str());
    };
    welcome_window->m_OpenProjectCallback = [this]() {
      project_manager->m_ProjectCreation = false;
      CherryGUI::SetWindowFocus(project_manager->GetAppWindow()->m_IdName.c_str());
    };
    welcome_window->m_SettingsCallback = [this]() {
      SetMainSettingsVisibility(true);
      CherryGUI::SetWindowFocus(system_settings->GetAppWindow()->m_IdName.c_str());
    };

    welcome_window->m_ProjectCallback = [this](const std::shared_ptr<EnvProject> &project) {
      project_manager->m_ProjectCreation = false;
      project_manager->m_SelectedEnvproject = project;
      CherryGUI::SetWindowFocus(project_manager->GetAppWindow()->m_IdName.c_str());
    };*/

    // Project manager
    // project_manager = ProjectManager::Create("?loc:loc.window_names.project_manager");
    // Cherry::AddAppWindow(project_manager->GetAppWindow());

    // Download center (change to Vortex Lab integratio)
    /*download_center = DownloadCenter::Create("Download Center");
    download_center->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(download_center->GetAppWindow());*/

    // Main settings
    system_settings = MainSettings::Create("?loc:loc.window_names.settings");
    system_settings->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(system_settings->GetAppWindow());

    // Logs window
    logs_window = LauncherLogUtility::Create("?loc:loc.window_names.logs_utility");
    logs_window->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(logs_window->GetAppWindow());

    // Logic content manager
    logic_content_manager = LogicalContentManager::Create("?loc:loc.window_names.logic_content_manager");
    logic_content_manager->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(logic_content_manager->GetAppWindow());

    // Content manager
    content_manager = ContentManager::Create("?loc:loc.window_names.content_manager");
    content_manager->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(content_manager->GetAppWindow());

    // Version manager
    version_manager = VersionManager::Create("?loc:loc.window_names.version_manager");
    version_manager->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(version_manager->GetAppWindow());

    std::string version = VORTEXLAUNCHER_VERSION;
    std::thread([this, version]() {
      VxContext *ctx = VortexMaker::GetCurrentContext();
      std::cout << "V vs Vlatest" << version << " -> " << ctx->latest_launcher_version.version << std::endl;
      if (VortexMaker::IsVersionGreater(version, ctx->latest_launcher_version.version)) {
        ctx->launcher_update_available = true;
      }
    }).detach();
  };

  void SetLogsVisibility(const bool &visibility) {
    logs_window->GetAppWindow()->SetVisibility(visibility);
  }

  void SetMainSettingsVisibility(const bool &visibility) {
    system_settings->GetAppWindow()->SetVisibility(visibility);
  }

  void SetWelcomeWindowVisibility(const bool &visibility) {
    welcome_window->GetAppWindow()->SetVisibility(visibility);
  }

  /*void SetDownloadCenterVisibility(const bool &visibility) {
    download_center->GetAppWindow()->SetVisibility(visibility);
    if (visibility) {
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
      spec.IconPath = Cherry::GetPath("resources/imgs/icon_update.png");
      download_center->GetAppWindow()->AttachOnNewWindow(spec);
    }
  }*/

  void SetAboutWindowVisibility(const bool &visibility) {
    about_window->GetAppWindow()->SetVisibility(visibility);
    if (visibility) {
      Cherry::ApplicationSpecification spec;

      std::string name = "About Vortex";
      spec.Name = name;
      spec.MinHeight = 100;
      spec.MinWidth = 200;
      spec.Height = 450;
      spec.DisableLogo = true;
      spec.DisableResize = true;
      spec.Width = 750;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = true;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
      spec.UniqueAppWindowName = about_window->GetAppWindow()->m_Name;

      spec.UsingCloseCallback = true;
      spec.CloseCallback = [this]() {
        Cherry::DeleteAppWindow(about_window->GetAppWindow());

        // Recreate a new sleepy instance
        about_window = AboutAppWindow::Create("?loc:loc.window_names.about");
        about_window->GetAppWindow()->SetVisibility(false);
        Cherry::AddAppWindow(about_window->GetAppWindow());
      };

      spec.MenubarCallback = []() { };
      spec.WindowSaves = false;
      about_window->GetAppWindow()->AttachOnNewWindow(spec);
    }
  }

  void SetLogicalContentManager(const bool &visibility) {
    logic_content_manager->GetAppWindow()->SetVisibility(visibility);
  }
  void SetContentManager(const bool &visibility) {
    content_manager->GetAppWindow()->SetVisibility(visibility);
  }

  void SetVersionManager(const bool &visibility) {
    version_manager->GetAppWindow()->SetVisibility(visibility);
  }

  void SetAboutWindow(const bool &visibility) {
    about_window->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetVersionManager() {
    return version_manager->GetAppWindow()->m_Visible;
  }

  bool GetContentManager() {
    return content_manager->GetAppWindow()->m_Visible;
  }

  bool GetLogicalContentManager() {
    return logic_content_manager->GetAppWindow()->m_Visible;
  }

  /*bool GetDownloadCenterVisibility() {
    return download_center->GetAppWindow()->m_Visible;
  }*/

  bool GetLogsVisibility() {
    return logs_window->GetAppWindow()->m_Visible;
  }

  bool GetMainSettingsVisibility() {
    return system_settings->GetAppWindow()->m_Visible;
  }

  bool GetWelcomeWindowVisibility() {
    return welcome_window->GetAppWindow()->m_Visible;
  }

  bool GetAboutAppWindowVisibility() {
    return about_window->GetAppWindow()->m_Visible;
  }

 private:
  std::shared_ptr<LauncherLogUtility> logs_window;
  std::shared_ptr<MainSettings> system_settings;
  // std::shared_ptr<DownloadCenter> download_center;
  std::shared_ptr<WelcomeWindow> welcome_window;
  std::shared_ptr<LogicalContentManager> logic_content_manager;
  std::shared_ptr<VersionManager> version_manager;
  std::shared_ptr<ContentManager> content_manager;
  std::shared_ptr<AboutAppWindow> about_window;

  bool offline = false;
  std::vector<std::string> topics = { "vortex1", "vortex2" };
};

static std::shared_ptr<Launcher> c_Launcher;

Cherry::Application *Cherry::CreateApplication(int argc, char **argv) {
  Cherry::ApplicationSpecification spec;
  std::shared_ptr<Layer> layer = std::make_shared<Layer>();

  std::string name = "Vortex Launcher";
  spec.Name = name;
  spec.MinHeight = 500;
  spec.MinWidth = 750;
  spec.Height = 600;
  spec.Width = 1150;
  spec.IconPath = Cherry::GetPath("resources/imgs/icon.png");
  spec.FavIconPath = Cherry::GetPath("resources/imgs/icon.png");
  spec.CustomTitlebar = true;
  spec.DisableWindowManagerTitleBar = true;
  spec.WindowOnlyClosable = true;
  spec.RenderMode = WindowRenderingMethod::SimpleWindow;
  spec.DefaultWindowName = "?loc:loc.window_names.welcome";
  spec.UniqueAppWindowName = "?loc:loc.window_names.welcome";
  spec.DisableTitle = true;
  spec.WindowSaves = false;
  spec.IconPath = Cherry::GetPath("resources/imgs/icon.png");
  spec.UseAudio();

  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::GetPath("resources/imgs/icon.png"));
  app->AddFont("Consola", Cherry::GetPath("resources/fonts/consola.ttf"), 17.0f);
  app->AddFont("ASMAN", Cherry::GetPath("resources/fonts/ASMAN.TFF"), 17.0f);
  app->AddFont("ClashBold", Cherry::GetPath("resources/fonts/ClashDisplay-Semibold.ttf"), 20.0f);
  app->AddFont("ClashMedium", Cherry::GetPath("resources/fonts/ClashDisplay-Medium.ttf"), 20.0f);
  app->AddFont("FiraCode", Cherry::GetPath("resources/fonts/FiraCode-Medium.ttf"), 20.0f);

  app->AddLocale("fr", Cherry::GetPath("resources/locales/fr.json"));
  app->AddLocale("en", Cherry::GetPath("resources/locales/en.json"));
  app->AddLocale("es", Cherry::GetPath("resources/locales/es.json"));
  app->AddLocale("pt", Cherry::GetPath("resources/locales/pt.json"));
  app->AddLocale("de", Cherry::GetPath("resources/locales/de.json"));
  app->SetDefaultLocale("en");
  app->SetLocale("en");

  /*static std::shared_ptr<Cherry::ImageButtonSimple> btn_close =
  std::make_shared<Cherry::ImageButtonSimple>("create_project_button",
  Cherry::GetPath("resources/imgs/icons/misc/icon_close.png")); btn_close->SetScale(0.20f);
  btn_close->SetInternalMarginX(1.0f);
  btn_close->SetInternalMarginY(1.0f);
  btn_close->SetLogoSize(5, 5);
  btn_close->SetBorderColorIdle("#00000000");
  btn_close->SetBackgroundColorClicked("#00000000");
  btn_close->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_close.png"));*/

  app->PushLayer(layer);
  app->SetFramebarCallback([=]() { CherryKit::ButtonText("Support"); });

  app->SetMenubarCallback([=]() {
    std::cout << "SetMenubarCallback" << std::endl;
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    std::cout << "BEFORME" << std::endl;

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

    std::cout << "sdh" << std::endl;
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    std::cout << "g" << std::endl;

    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
    std::cout << "qsf" << std::endl;

    static bool t;

    std::cout << "BEFORME" << std::endl;
    VxContext &ctx = *CVortexMaker;

    std::cout << "okl" << &ctx << std::endl;
    std::cout << "EB" << std::endl;

    CherryKit::NotificationButton(
        &ctx.launcher_update_available,
        10,
        "info",
        "Update Vortex Launcher",
        "A new update for the launcher is available !" + ctx.latest_launcher_version.version,
        []() {
          if (CherryKit::ButtonImageText("Update now", Cherry::GetPath("resources/imgs/icons/misc/icon_upgrade.png"))
                  .GetData("isClicked") == "true") {
            std::thread([]() {
              VortexMaker::OpenLauncherUpdater(
                  VortexMaker::GetCurrentContext()->m_VortexLauncherPath,
                  VortexMaker::GetCurrentContext()->IO.sys_vortexlauncher_dist);
            }).detach();
            Cherry::Application::Get().Close();
          }
        });

    CherryKit::NotificationSimple(
        &ctx.vortex_update_available,
        10,
        "info",
        "Vortex " + ctx.latest_vortex_version->version + " is live !",
        "A new update of Vortex is available ! Try it now" + ctx.latest_launcher_version.version);

    /*if(ctx.launcher_update_available)
    {
        static bool close_toast = false;

      ImGuiToast toast(ImGuiToastType::Info, 100000,

[](){
/*static std::shared_ptr<Cherry::ImageTextButtonSimple> btn = std::make_shared<Cherry::ImageTextButtonSimple>("UpdateButton",
"Update launcher"); btn->SetScale(0.85f); btn->SetInternalMarginX(10.0f); btn->SetLogoSize(15, 15);
btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_upgrade.png"));*


},
[]() { return (CherryKit::ButtonImageText("",
Cherry::GetPath("resources/imgs/icons/misc/icon_close.png")).GetData("isClicked") == "true"); },  // Wrap Render in a lambda
nullptr,
Cherry::GetTexture(Cherry::GetPath("resources/imgs/icon_update.png"))
);

      toast.setTitle("Update Vortex Launcher");
      toast.setContent("A new update for the launcher is available ! (%s)", ctx.latest_launcher_version.version.c_str());
      toast.setOnButtonPress([](){
        VortexMaker::OpenLauncherUpdater(VortexMaker::GetCurrentContext()->m_VortexLauncherPath,
VortexMaker::GetCurrentContext()->IO.sys_vortexlauncher_dist); Cherry::Application::Get().Close();
      });


      toast.setButtonLabel("Update now");

      Cherry::AddNotification(toast);
      ctx.launcher_update_available = false;
    }
*/
    /*if(ctx.vortex_update_available)
    {
      ImGuiToast toast(ImGuiToastType::None, 100000,  [](){
/*static std::shared_ptr<Cherry::ImageTextButtonSimple> create_project_button =
std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "Create a project");
create_project_button->SetScale(0.85f);
create_project_button->SetInternalMarginX(10.0f);
create_project_button->SetLogoSize(15, 15);
create_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_upgrade.png"));*
      },
[]() { return (CherryKit::ButtonImageText("",
Cherry::GetPath("resources/imgs/icons/misc/icon_close.png")).GetData("isClicked") == "true"); }
);
      toast.setTitle("Vortex %s is live !", ctx.latest_vortex_version.version.c_str());
      toast.setContent("A new update of Vortex is available ! Try it now");
      toast.setOnButtonPress([](){
        // Go on the vortex version download page....
      });
      toast.setButtonLabel("Update now");

      Cherry::AddNotification(toast);
      ctx.vortex_update_available = false;
    }*/

    if (CherryGUI::BeginMenu(Cherry::GetLocale("loc.menubar.menu.vortex").c_str())) {
      CherryKit::SeparatorText(Cherry::GetLocale("loc.menubar.summary.general"));

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.update_vortex").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.update_vortex_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_update.png")),
              false)) {
        std::thread([]() {
          VortexMaker::OpenLauncherUpdater(
              VortexMaker::GetCurrentContext()->m_VortexLauncherPath,
              VortexMaker::GetCurrentContext()->IO.sys_vortexlauncher_dist);
        }).detach();

        // Cherry::Application::Get().Close();
      }

      CherryKit::SeparatorText(Cherry::GetLocale("loc.menubar.summary.vortex_labs"));

      CherryGUI::BeginDisabled();
      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.vortex_labs").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.vortex_labs_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_net.png")),
              false)) {
        // c_Launcher->SetDownloadCenterVisibility(!c_Launcher->GetDownloadCenterVisibility());
      }
      CherryGUI::EndDisabled();

      CherryKit::SeparatorText(Cherry::GetLocale("loc.menubar.summary.manage"));

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.logical_contents").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.logical_contents_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png")),
              c_Launcher->GetLogicalContentManager())) {
        c_Launcher->SetLogicalContentManager(!c_Launcher->GetLogicalContentManager());
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.static_contents").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.static_contents_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
              c_Launcher->GetContentManager())) {
        c_Launcher->SetContentManager(!c_Launcher->GetContentManager());
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.vortex_versions").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.vortex_versions_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_versions.png")),
              c_Launcher->GetVersionManager())) {
        c_Launcher->SetVersionManager(!c_Launcher->GetVersionManager());
      }

      CherryKit::SeparatorText(Cherry::GetLocale("loc.menubar.summary.other"));

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.about_vortex").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.about_vortex_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
              c_Launcher->GetLogsVisibility())) {
        c_Launcher->SetAboutWindowVisibility(!c_Launcher->GetAboutAppWindowVisibility());
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.about_contributors").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.about_contributors_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_people.png")),
              false)) {
        VortexMaker::OpenURL("https://vortex.infinite.si/contributors");
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.documentation").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.documentation_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_book.png")),
              c_Launcher->GetMainSettingsVisibility())) {
        VortexMaker::OpenURL("https://vortex.infinite.si/learn");
      }

      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu(Cherry::GetLocale("loc.menubar.menu.options").c_str())) {
      CherryGUI::GetFont()->Scale *= 0.8;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
      CherryGUI::Text(Cherry::GetLocale("loc.menubar.summary.utilities").c_str());
      CherryGUI::PopStyleColor();

      CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
      CherryGUI::Separator();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = 0.84;
      CherryGUI::PopFont();
      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.logs").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.logs_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png")),
              c_Launcher->GetLogsVisibility())) {
        c_Launcher->SetLogsVisibility(!c_Launcher->GetLogsVisibility());
      }

      CherryGUI::BeginDisabled();
      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.session_utility").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.session_utility_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_list.png")),
              c_Launcher->GetLogsVisibility())) {
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.latest_crash").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.latest_crash_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
              c_Launcher->GetLogsVisibility())) {
      }

      CherryGUI::EndDisabled();

      CherryGUI::GetFont()->Scale *= 0.8;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
      CherryGUI::Text(Cherry::GetLocale("loc.menubar.summary.settings").c_str());
      CherryGUI::PopStyleColor();

      CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
      CherryGUI::Separator();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = 0.84;
      CherryGUI::PopFont();
      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.settings").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.settings_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png")),
              c_Launcher->GetMainSettingsVisibility())) {
        c_Launcher->SetMainSettingsVisibility(!c_Launcher->GetMainSettingsVisibility());
      }

      if (CherryGUI::BeginMenu("Switch language")) {
        if (CherryGUI::MenuItem(
                "English",
                "Switch language to english (en)",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/us.png")),
                c_Launcher->GetMainSettingsVisibility())) {
          app->SetLocale("en");
        }
        if (CherryGUI::MenuItem(
                "Français",
                "Changer la langue pour le Français (fr)",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/fr.png")),
                c_Launcher->GetMainSettingsVisibility())) {
          app->SetLocale("fr");
        }
        if (CherryGUI::MenuItem(
                "Español",
                "Cambiar el idioma a español",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/es.png")),
                c_Launcher->GetMainSettingsVisibility())) {
          app->SetLocale("es");
        }
        if (CherryGUI::MenuItem(
                "Português",
                "Alterar o idioma para Português",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/po.png")),
                c_Launcher->GetMainSettingsVisibility())) {
          app->SetLocale("pt");
        }
        if (CherryGUI::MenuItem(
                "Deutsch",
                "Ändern Sie die Sprache auf Deutsch",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/po.png")),
                c_Launcher->GetMainSettingsVisibility())) {
          app->SetLocale("de");
        }

        CherryGUI::EndMenu();
      }
      CherryGUI::EndMenu();
    }

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);
  });

  c_Launcher = std::make_shared<Launcher>();

  return app;
}

#endif  // VORTEXLAUNCHER_LAUNCHER_H