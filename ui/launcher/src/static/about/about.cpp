#include "./about.hpp"

#include <openssl/sha.h>

std::string getBuildDate() {
  return BUILD_DATE_STR;
}

std::string getGitCommit() {
  return GIT_COMMIT_HASH;
}

std::string getVortexLauncherDist() {
  return BUILD_DIST;
}

#ifdef _WIN32
#define VORTEX_EXECUTABLE "vortex_launcher.exe"
#else
#define VORTEX_EXECUTABLE "vortex_launcher"
#endif

static std::string vxl_exehash = "";
static std::string system_desktop = "";

std::string computeSHA256Short(const std::string &filepath, size_t length = 10) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file)
    return "";

  SHA256_CTX sha256;
  SHA256_Init(&sha256);

  std::vector<char> buffer(8192);
  while (file.read(buffer.data(), buffer.size()) || file.gcount()) {
    SHA256_Update(&sha256, buffer.data(), file.gcount());
  }

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_Final(hash, &sha256);

  std::ostringstream oss;
  for (size_t i = 0; i < SHA256_DIGEST_LENGTH && oss.tellp() < length; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }

  std::string result = oss.str();
  return result.substr(0, length);
}

std::string getVortexLauncherHash() {
  if (vxl_exehash.empty()) {
    std::string exename = VORTEX_EXECUTABLE;
    std::string exepath = Cherry::GetPath(exename);
    if (std::filesystem::exists(exepath)) {
      vxl_exehash = computeSHA256Short(exepath);
    }
  }
  return vxl_exehash;
}

#if defined(_WIN32)
#define OS_NAME "Windows"
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_NAME "macOS"
#elif defined(__linux__)
std::string getLinuxDistroName() {
  std::ifstream file("/etc/os-release");
  std::string line;
  std::string distro = "Linux";

  while (std::getline(file, line)) {
    if (line.rfind("PRETTY_NAME=", 0) == 0) {
      std::string value = line.substr(13, line.size() - 14);  // remove PRETTY_NAME=" and ending "
      distro = "Linux (" + value + ")";
      break;
    }
  }

  return distro;
}
std::string getLinuxDesktopEnvAndDisplayServer() {
  const char *xdgDesktop = std::getenv("XDG_CURRENT_DESKTOP");
  const char *desktopSession = std::getenv("DESKTOP_SESSION");
  const char *waylandDisplay = std::getenv("WAYLAND_DISPLAY");
  const char *x11Display = std::getenv("DISPLAY");

  std::string de;
  if (xdgDesktop) {
    de = xdgDesktop;
  } else if (desktopSession) {
    de = desktopSession;
  } else {
    de = "Unknown DE";
  }

  std::string compositor;
  if (waylandDisplay) {
    compositor = "Wayland";
  } else if (x11Display) {
    compositor = "X11";
  } else {
    compositor = "Unknown Display";
  }

  return de + " " + compositor;
}

#define OS_NAME getLinuxDistroName().c_str()
#else
#define OS_NAME "Unknown OS"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_NAME "x86_64"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_NAME "ARM64"
#elif defined(__i386__) || defined(_M_IX86)
#define ARCH_NAME "x86"
#else
#define ARCH_NAME "Unknown Arch"
#endif

namespace VortexLauncher {
  AboutAppWindow::AboutAppWindow(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    m_AppWindow->SetInternalPaddingX(8.0f);
    m_AppWindow->SetInternalPaddingY(8.0f);

    
#if defined(__linux__)
    system_desktop = " - " + getLinuxDesktopEnvAndDisplayServer();
    #endif

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::shared_ptr<Cherry::AppWindow> &AboutAppWindow::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<AboutAppWindow> AboutAppWindow::Create(const std::string &name) {
    auto instance = std::shared_ptr<AboutAppWindow>(new AboutAppWindow(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void AboutAppWindow::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  void AboutAppWindow::Render() {
    float window_width = ImGui::GetWindowSize().x;
    float image_height = window_width / 3.435f;

    ImGui::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(window_width, image_height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));  // CherryStyle::Padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));   // CherryStyle::Padding
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
    ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryStyle::AddMarginX(20.0f);
    CherryStyle::AddMarginY(5.0f);
    ImGui::BeginChild("aboutchild", ImVec2(430, 0));  // cherry api

    Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
    CherryKit::TextSimple("Vortex Launcher");

    std::string vortex_launcher_version = VORTEXLAUNCHER_VERSION;
    std::string cherry_version = CHERRY_VERSION;
    std::string os_name = OS_NAME;
    std::string os_arch = ARCH_NAME;

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Version: " + vortex_launcher_version);

    CherryKit::Space(12.0f);

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Cherry version: " + cherry_version);
    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Build: " + getBuildDate() + " (" + getVortexLauncherDist() + ")");
    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple(
        "Hash: "
        " exe(" +
        getVortexLauncherHash() + ") git(" + getGitCommit() + ")");

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("System: " + os_name + " " + os_arch  + system_desktop);

    ImGui::EndChild();
    CherryGUI::SameLine();

    ImGui::BeginChild("link");  // cherry api

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");

    if (CherryKit::ButtonImageText("Support Us", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") ==
        "true") {
    }

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");
    if (CherryKit::ButtonImageText("Credits & Contributors", Cherry::GetPath("resources/base/add.png"))
            ->GetData("isClicked") == "true") {
    }

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");
    if (CherryKit::ButtonImageText("Learn and Documentation", Cherry::GetPath("resources/base/add.png"))
            ->GetData("isClicked") == "true") {
    }

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Vortex is a open and free software.");
    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Licensed under the  Apache-2.0 license ");
    CherryKit::Space(12.0f);

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Never stop hacking !");

    ImGui::EndChild();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);  // CherryStyle::Padding
  }

}  // namespace VortexLauncher
