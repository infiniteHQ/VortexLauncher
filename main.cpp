#define SDL_MAIN_HANDLED
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

// UI instances
#include "./lib/restcpp/include/restclient-cpp/restclient.h"
#include "./main/include/modules/load.h"
#include "./main/include/plugins/load.h"
#include "./main/include/templates/load.h"
#include "./main/include/vortex.h"
#include "./ui/launcher/launcher.hpp"

static std::string session_id = "unknow";

// TODO vortex -i --module --path
// TODO vortex -d --module --url

void PrintInfinite() {
  std::cout << R"(
                  ███  ███   ███  ███████▓▒███ ███░  ░███ ███▓█████████ ████████                   
                  ███  ███▒  ███  ███████▓▒███ █████ ░███ ███▓█████████ ████████                   
                  ███  ████  ███  ███▓▓▓▓ ▒███ ██████▒███ ███▓   ███░   ███                        
                  ███  █████ ███  ███████ ▒███ ███▒██████ ███▓   ███░   ███                        
                  ███  █████████  ███     ▒███ ███▒ █████ ███▓   ███░   ███                        
                  ███  ███▓█████  ███     ▒███ ███▒  ████ ███▓   ███░   ███                        
                  ███  ███ █████  ███                     ███▓   ███░   ███████                    
                  ███  ███ ▒████  ██░                        ░   ███░   ███████                    
                  ███  ███  ████                                 ███░   ███                        
                  ███  ███  ░█▓                                    █░   ███                        
                  ███  ███                                              ███                        
                  ███  ███                                              ███▒                       
                  ███  ██                                               ░████                      
                  ███                                                     ████▒                    
                  ███                                                       ███░                   
                  ██▒                                                        ███                   
                  █░                                                          ██                   
                  █                                                            █                   
                                                                                                              )"
            << std::endl;
}

void PrintHeader() {
  // Print this every time
  std::cout << std::endl;
  std::cout << "\033[38;2;177;255;49m";
  PrintInfinite();
  std::cout << "\033[0m";

  std::cout << "\033[38;2;177;255;49m"
            << " Vortex Launcher " << VORTEXLAUNCHER_VERSION << "\033[0m" << std::endl;

  std::cout << "┌────────────────────────────────────────────────────────────────────────────────────────────────┐"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Description : "
            << "\033[0m"
            << "                                                                                 │ " << std::endl;

  std::cout << "│ ❓ Vortex is a complete open creation platform that contain a bunch of tools for creators and  │ "
            << std::endl;
  std::cout << "│  makers.                                                                                       │ "
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤"
            << std::endl;

  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Usage : "
            << "\033[0m"
            << "vortex <paramater(s)...> <information(s)...>                                           │ " << std::endl;

  std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Helpfull commands : "
            << "\033[0m"
            << "                                                                           │ " << std::endl;
  std::cout << "│ -h   --help :                          See all parameters                                      │ "
            << std::endl;
  std::cout << "│ -i   --install :                       Install contents/components on the system               │ "
            << std::endl;
  std::cout << "│ -d   --download :                      Download contents/components to install on the system   │ "
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Links : "
            << "\033[0m"
            << "                                                                                       │ " << std::endl;
  std::cout << "│ 🌍 Vortex website : https://infinite.si/                                                       │ "
            << std::endl;
  std::cout << "│ 📝 News : https://infinite.si/blog                                                             │ "
            << std::endl;
  std::cout << "│ 💬 Discord : https://discord.gg/H2wptkecUg                                                     │ "
            << std::endl;
  std::cout
      << "│ 🧑‍💻 GitHub : https://github.com/infiniteHQ/Vortex                                               │ "
      << std::endl;
  std::cout << "└────────────────────────────────────────────────────────────────────────────────────────────────┘ "
            << std::endl;
  std::cout << std::endl;
}

VxContext *InitBlankRuntime(bool logger) {
  VxContext *ctx = VortexMaker::CreateContext();

  ctx->state.session_id = session_id;
  ctx->version = VORTEXLAUNCHER_VERSION;

  VortexMaker::InitializePlatformVendor();
  VortexMaker::CreateGlobalLogger();
  VortexMaker::CreateConsoleLogger();
  VortexMaker::LogInfo("Bootstrapp", "Initializing runtime...");

  VortexMaker::CreateSessionTopic(ctx->state.session_id);

  // Initialize environment
  VortexMaker::InitEnvironment();
  VortexMaker::DetectPlatform();
  VortexMaker::DetectArch();

  ctx->m_VortexLauncherPath = Cherry::GetPath("");

  // Refresh environment pools
  VortexMaker::RefreshEnvironmentProjectsPools();
  VortexMaker::RefreshEnvironmentModulesPools();
  VortexMaker::RefreshEnvironmentTemplatesPools();
  VortexMaker::RefreshEnvironmentPluginsPools();
  VortexMaker::RefreshEnvironmentVortexVersionsPools();

  VortexMaker::RefreshVortexLauncherDists();
  VortexMaker::RefreshVortexDists();

  VortexMaker::UpdateVortexWebData();
  VortexMaker::UpdateVortexLauncherWebData();
  VortexMaker::UpdateVortexNews({ "vortex1", "vortex2" });

  VortexMaker::RefreshEnvironmentProjects();
  VortexMaker::RefreshEnvironmentVortexVersion();

  VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);

  VortexMaker::LoadSystemModules(ctx->IO.sys_em);
  VortexMaker::LoadSystemPlugins(ctx->IO.sys_ep);
  ctx->logger = logger;
  return ctx;
}

/**
 * @brief : Entry point of main Vortex runtime command.
 */

int main(int argc, char *argv[]) {
  // Check command-line arguments for --install, -i, --download, -d
  bool installFlag = false;
  bool downloadFlag = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-i" || arg == "--install") {
      installFlag = true;
    } else if (arg == "-d" || arg == "--download") {
      downloadFlag = true;
    } else if (arg == "--test" || arg == "--download") {
      std::cout << "OK" << std::endl;
      return 0;
    }
  }
  PrintHeader();
  VxContext *ctx = InitBlankRuntime(true);

  // Checking connexion
  RestClient::Response r = RestClient::get("http://api.infinite.si:8000/");
  if (r.code != 200) {
    ctx->disconnected = true;
  }

  VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");

  std::thread mainthread;
  std::thread Thread([&]() { Cherry::Main(argc, argv); });
  mainthread.swap(Thread);

  std::cout << r.body << std::endl;

  mainthread.join();

  return 0;
}