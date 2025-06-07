#include "./about.hpp"

#ifdef _WIN32
#else
#include <openssl/sha.h>
#endif


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

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

#pragma comment(lib, "advapi32.lib")

std::string computeSHA256Short(const std::string &filepath, size_t length = 10) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
        return "";

    std::vector<char> buffer(8192);
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE hash[32]; // SHA-256 produces 32-byte hash
    DWORD hashLen = sizeof(hash);

    if (!CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }

    while (file.read(buffer.data(), buffer.size()) || file.gcount()) {
        if (!CryptHashData(hHash, reinterpret_cast<BYTE*>(buffer.data()), static_cast<DWORD>(file.gcount()), 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return "";
        }
    }

    if (!CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    std::ostringstream oss;
    for (DWORD i = 0; i < hashLen && oss.tellp() < (std::streampos)length * 2; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    std::string result = oss.str();
    return result.substr(0, length);
}
#else
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
#endif

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
std::string GetWindowsVersion() {
    HKEY hKey;
    LONG lRes = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
        KEY_READ, &hKey);

    if (lRes != ERROR_SUCCESS)
        return "Unknown Windows version";

    char productName[256];
    DWORD size = sizeof(productName);
    RegQueryValueExA(hKey, "ProductName", NULL, NULL, (LPBYTE)productName, &size);

    DWORD buildNumber = 0;
    size = sizeof(buildNumber);
    RegQueryValueExA(hKey, "CurrentBuildNumber", NULL, NULL, (LPBYTE)&buildNumber, &size);

    char releaseId[256] = {0};
    size = sizeof(releaseId);
    RegQueryValueExA(hKey, "DisplayVersion", NULL, NULL, (LPBYTE)releaseId, &size);

    RegCloseKey(hKey);

    std::string versionStr = std::string(productName) + " " + std::string(releaseId);
    return versionStr;
}

#define OS_NAME GetWindowsVersion().c_str()
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

    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

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
    float window_width = CherryGUI::GetWindowSize().x;
    float image_height = window_width / 3.435f;

    CherryGUI::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(window_width, image_height));

    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));  // CherryStyle::Padding
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));   // CherryStyle::Padding
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryStyle::AddMarginX(20.0f);
    CherryStyle::AddMarginY(5.0f);
    CherryGUI::BeginChild("aboutchild", ImVec2(430, 0));  // cherry api

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
    CherryKit::TextSimple("System: " + os_name + " " + os_arch + system_desktop);

    CherryGUI::EndChild();
    CherryGUI::SameLine();

    CherryGUI::BeginChild("link");  // cherry api

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");

    if (CherryKit::ButtonImageTextImage(
            "Support Us",
            Cherry::GetPath("resources/imgs/icons/launcher/heart.png"),
            Cherry::GetPath("resources/imgs/weblink.png"))
            ->GetData("isClicked") == "true") {
      VortexMaker::OpenURL("https://fund.infinite.si/");
    }

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");
    if (CherryKit::ButtonImageTextImage(
            "Credits & Contributors",
            Cherry::GetPath("resources/imgs/icons/misc/icon_people.png"),
            Cherry::GetPath("resources/imgs/weblink.png"))
            ->GetData("isClicked") == "true") {
      VortexMaker::OpenURL("https://github.com/infiniteHQ/Vortex/graphs/contributors");
    }

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");
    if (CherryKit::ButtonImageTextImage(
            "Learn and Documentation",
            Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
            Cherry::GetPath("resources/imgs/weblink.png"))
            ->GetData("isClicked") == "true") {
      VortexMaker::OpenURL("https://vortex.infinite.si/learn");
    }

    CherryKit::Space(1.0f);
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple("Vortex is a open and free software.");
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple("Licensed under the  Apache-2.0 license ");
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple("Never stop hacking !");

    CherryGUI::EndChild();

    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(2);  // CherryStyle::Padding
  }

}  // namespace VortexLauncher
