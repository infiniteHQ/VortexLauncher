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
    welcome_window->m_ProjectCallback = [this](const std::shared_ptr<EnvProject> &project) {
      welcome_window->m_SelectedChildName = "?loc:loc.windows.welcome.open_project";
      welcome_window->m_SelectedEnvproject = project;
    };
    welcome_window->m_SettingsCallback = [this]() { this->SpawnMainSettings(); };

    Cherry::AddAppWindow(welcome_window->GetAppWindow());
    /*welcome_window->m_CreateProjectCallback = [this]() {
      project_manager->m_ProjectCreation = true;
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

    std::string version = VORTEXLAUNCHER_VERSION;
    std::thread([this, version]() {
      VxContext *ctx = VortexMaker::GetCurrentContext();
      std::cout << "V vs Vlatest" << version << " -> " << ctx->latest_launcher_version.version << std::endl;
      if (VortexMaker::IsVersionGreater(version, ctx->latest_launcher_version.version)) {
        ctx->launcher_update_available = true;
      }
    }).detach();
  };

  void SpawnLogicContentManager() {
    std::string label = "?loc:loc.window_names.logic_content_manager" + std::to_string(logic_content_manager_counter);
    auto settings_win = LogicalContentManager::Create(label);
    settings_win->GetAppWindow()->SetVisibility(true);

    Cherry::ApplicationSpecification spec;

    std::string name = "Manage logical contents";
    spec.Name = name;
    spec.MinHeight = 500;
    spec.MinWidth = 500;
    spec.Height = 800;
    spec.DisableResize = true;
    spec.Width = 1350;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = settings_win->GetAppWindow()->m_Name;

    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, settings_win]() {
      Cherry::DeleteAppWindow(settings_win->GetAppWindow());
      logic_content_manager_counter--;
    };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;

    Cherry::AddAppWindow(settings_win->GetAppWindow());
    settings_win->GetAppWindow()->AttachOnNewWindow(spec);
    logic_content_manager_counter++;
  }

  void SpawnContentManager() {
    std::string label = "?loc:loc.window_names.content_manager" + std::to_string(content_manager_counter);
    auto settings_win = ContentManager::Create(label);
    settings_win->GetAppWindow()->SetVisibility(true);

    Cherry::ApplicationSpecification spec;

    std::string name = "Manage contents";
    spec.Name = name;
    spec.MinHeight = 500;
    spec.MinWidth = 500;
    spec.Height = 800;
    spec.DisableResize = true;
    spec.Width = 1350;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = settings_win->GetAppWindow()->m_Name;

    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, settings_win]() {
      Cherry::DeleteAppWindow(settings_win->GetAppWindow());
      content_manager_counter--;
    };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;

    Cherry::AddAppWindow(settings_win->GetAppWindow());
    settings_win->GetAppWindow()->AttachOnNewWindow(spec);
    content_manager_counter++;
  }

  void SpawnLogsUtility() {
    std::string label = "?loc:loc.window_names.logs_utility" + std::to_string(logs_utility_counter);
    auto settings_win = LauncherLogUtility::Create(label);
    settings_win->GetAppWindow()->SetVisibility(true);

    Cherry::ApplicationSpecification spec;

    std::string name = "Logs Utility";
    spec.Name = name;
    spec.MinHeight = 500;
    spec.MinWidth = 500;
    spec.Height = 800;
    spec.DisableResize = true;
    spec.Width = 1350;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = settings_win->GetAppWindow()->m_Name;

    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, settings_win]() {
      Cherry::DeleteAppWindow(settings_win->GetAppWindow());
      logs_utility_counter--;
    };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;

    Cherry::AddAppWindow(settings_win->GetAppWindow());
    settings_win->GetAppWindow()->AttachOnNewWindow(spec);
    logs_utility_counter++;
  }

  void SpawnVersionManager() {
    std::string label = "?loc:loc.window_names.version_manager" + std::to_string(vortex_versions_counter);
    auto settings_win = VersionManager::Create(label);
    settings_win->GetAppWindow()->SetVisibility(true);

    Cherry::ApplicationSpecification spec;

    std::string name = "Version Manager";
    spec.Name = name;
    spec.MinHeight = 500;
    spec.MinWidth = 500;
    spec.Height = 800;
    spec.DisableResize = true;
    spec.Width = 1350;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = settings_win->GetAppWindow()->m_Name;

    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, settings_win]() {
      Cherry::DeleteAppWindow(settings_win->GetAppWindow());
      vortex_versions_counter--;
    };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;

    Cherry::AddAppWindow(settings_win->GetAppWindow());
    settings_win->GetAppWindow()->AttachOnNewWindow(spec);
    vortex_versions_counter++;
  }

  void SpawnMainSettings() {
    std::string label = "?loc:loc.window_names.settings" + std::to_string(system_settings_counter);
    auto settings_win = MainSettings::Create(label);
    settings_win->GetAppWindow()->SetVisibility(true);

    Cherry::ApplicationSpecification spec;

    std::string name = "Main settings";
    spec.Name = name;
    spec.MinHeight = 500;
    spec.MinWidth = 500;
    spec.Height = 800;
    spec.DisableResize = true;
    spec.Width = 1350;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = settings_win->GetAppWindow()->m_Name;

    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, settings_win]() {
      Cherry::DeleteAppWindow(settings_win->GetAppWindow());
      system_settings_counter--;
    };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;

    Cherry::AddAppWindow(settings_win->GetAppWindow());
    settings_win->GetAppWindow()->AttachOnNewWindow(spec);
    system_settings_counter++;
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

  void SetAboutWindow(const bool &visibility) {
    about_window->GetAppWindow()->SetVisibility(visibility);
  }

  /*bool GetDownloadCenterVisibility() {
    return download_center->GetAppWindow()->m_Visible;
  }*/

  bool GetWelcomeWindowVisibility() {
    return welcome_window->GetAppWindow()->m_Visible;
  }

  bool GetAboutAppWindowVisibility() {
    return about_window->GetAppWindow()->m_Visible;
  }

 private:
  int system_settings_counter = 0;
  int vortex_versions_counter = 0;
  int logs_utility_counter = 0;
  int logic_content_manager_counter = 0;
  int content_manager_counter = 0;
  // std::shared_ptr<DownloadCenter> download_center;
  std::shared_ptr<WelcomeWindow> welcome_window;
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

  spec.SetFramebarCallback([]() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 40.0f);
    if (VortexMaker::GetCurrentContext()->disconnected) {
      CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/icons/misc/icon_disconnected.png"), 15.0f, 15.0f);
    } else {
      CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/icons/misc/icon_connected.png"), 15.0f, 15.0f);
    }
    CherryNextComponent.SetProperty("padding_x", "2");
    CherryNextComponent.SetProperty("padding_y", "0");
    CherryNextComponent.SetProperty("color_bg", "#66364988");
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_text", "#CBCBCB");
    if (CherryKit::ButtonImageText("Support", Cherry::GetPath("resources/imgs/icons/launcher/heart.png"))
            .GetDataAs<bool>("isClicked")) {
      VortexMaker::OpenURL("https://fund.infinite.si/");
    }
  });

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

  app->SetMenubarCallback([=]() {
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
    VxContext &ctx = *CVortexMaker;
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
              false)) {
        c_Launcher->SpawnLogicContentManager();
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.static_contents").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.static_contents_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
              false)) {
        c_Launcher->SpawnContentManager();
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.vortex_versions").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.vortex_versions_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_versions.png")),
              false)) {
        c_Launcher->SpawnVersionManager();
      }

      CherryKit::SeparatorText(Cherry::GetLocale("loc.menubar.summary.other"));

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.about_vortex").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.about_vortex_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
              false)) {
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
              false)) {
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
              false)) {
        c_Launcher->SpawnLogsUtility();
      }

      CherryGUI::BeginDisabled();
      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.session_utility").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.session_utility_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_list.png")),
              false)) {
      }

      if (CherryGUI::MenuItem(
              Cherry::GetLocale("loc.menubar.menuitem.latest_crash").c_str(),
              Cherry::GetLocale("loc.menubar.menuitem.latest_crash_desc").c_str(),
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
              false)) {
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
              false)) {
        c_Launcher->SpawnMainSettings();
      }

      if (CherryGUI::BeginMenu("Switch language")) {
        if (CherryGUI::MenuItem(
                "English",
                "Switch language to english (en)",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/us.png")),
                false)) {
          app->SetLocale("en");
        }
        if (CherryGUI::MenuItem(
                "Français",
                "Changer la langue pour le Français (fr)",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/fr.png")),
                false)) {
          app->SetLocale("fr");
        }
        if (CherryGUI::MenuItem(
                "Español",
                "Cambiar el idioma a español",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/es.png")),
                false)) {
          app->SetLocale("es");
        }
        if (CherryGUI::MenuItem(
                "Português",
                "Alterar o idioma para Português",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/po.png")),
                false)) {
          app->SetLocale("pt");
        }
        if (CherryGUI::MenuItem(
                "Deutsch",
                "Ändern Sie die Sprache auf Deutsch",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/flags/po.png")),
                false)) {
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