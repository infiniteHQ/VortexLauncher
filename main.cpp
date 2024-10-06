#include <iostream>
#include <string>
#include <thread>
#include <fstream>

// UI instances
#include "./ui/launcher/launcher.hpp"

#include "./main/include/vortex.h"

static std::string session_id = "unknow";

void PrintInfinite()
{
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

void PrintHeader()
{
    // Print this every time
    std::cout << std::endl;
    std::cout << "\033[38;2;177;255;49m";
    PrintInfinite();
    std::cout << "\033[0m";

    std::cout << "\033[38;2;177;255;49m"
              << " Vortex " << VORTEX_VERSION
              << "\033[0m" << std::endl;

    std::cout << "┌────────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Description : "
              << "\033[0m"
              << "                                                                                 │ " << std::endl;

    std::cout << "│ ❓ Vortex is a complete open creation platform that contain a bunch of tools for creators and  │ " << std::endl;
    std::cout << "│  makers.                                                                                       │ " << std::endl;
    std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;

    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Usage : "
              << "\033[0m"
              << "vortex <paramater(s)...> <information(s)...>                                           │ " << std::endl;

    std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Helpfull commands : "
              << "\033[0m"
              << "                                                                           │ " << std::endl;
    std::cout << "│ -h   --help :                          See all parameters                                      │ " << std::endl;
    std::cout << "│ -l   --launcher :                      Open the Vortex launcher (for projects, components...)  │ " << std::endl;
    std::cout << "│ -e   --editor :                        Open the Vortex graphical interface                     │ " << std::endl;
    std::cout << "│ -cp  --create-project  <...> :         Create a new project                                    │ " << std::endl;
    std::cout << "│ -i   --install  <...> :                Install a ressource into the user environment           │ " << std::endl;
    std::cout << "│ -bi  --build-install  <...> :          Build & Install a ressource into the user environment   │ " << std::endl;
    std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Links : "
              << "\033[0m"
              << "                                                                                       │ " << std::endl;
    std::cout << "│ 🌍 Vortex website : https://infinite.si/                                                       │ " << std::endl;
    std::cout << "│ 📝 News : https://infinite.si/blog                                                             │ " << std::endl;
    std::cout << "│ 💬 Discord : https://discord.gg/H2wptkecUg                                                     │ " << std::endl;
    std::cout << "│ 🧑‍💻 GitHub : https://github.com/infiniteHQ/Vortex                                               │ " << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────────────────────────────────────┘ " << std::endl;
    std::cout << std::endl;
}

/**
 * @brief : Entry point of main Vortex runtime command.
 */
int main(int argc, char *argv[])
{
    PrintInfinite();

  std::thread mainthread;
  std::thread Thread([&]()
                     { Cherry::Main(argc, argv); });
  mainthread.swap(Thread);

  while (g_ApplicationRunning)
  {
    /* Your program loop... */
  }

  mainthread.join();
  
    return 0;
}