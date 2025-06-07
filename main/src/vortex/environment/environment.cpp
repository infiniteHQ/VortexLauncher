#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

namespace fs = std::filesystem;

VORTEX_API void VortexMaker::CheckBlankProject() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  for (auto template_pool : ctx.IO.sys_templates_pools) {
    std::string blank_project_path;

#ifdef _WIN32
    blank_project_path = "\\blank_project";
#else
    blank_project_path = "/blank_project";
#endif

    if (!std::filesystem::exists(template_pool + blank_project_path)) {
      ctx.blankproject_exist = true;
    }
  }
}

VORTEX_API void VortexMaker::InitEnvironment() {
  CheckBlankProject();
  std::string homeDir = VortexMaker::getHomeDirectory();

  std::string vxBasePath;
  std::string vortexProjectsPath;

  if (VortexMaker::IsWindows()) {
    vxBasePath = homeDir + "\\.vx\\";
    vortexProjectsPath = homeDir + "\\VortexProjects\\";
  } else {
    vxBasePath = homeDir + "/.vx/";
    vortexProjectsPath = homeDir + "/VortexProjects/";
  }

  {
    std::string path = vxBasePath + "data/";
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string path = vxBasePath + "data/";
    std::string file = path + "launcher_data.json";

    nlohmann::json default_data = { { "latest_saved_version", nlohmann::json::object() } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "sessions.json";

    nlohmann::json default_data = { { "KeepSessionDataTimeout", "30d" } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "contents\\";
#else
    endpath = "contents/";
#endif

    std::string path = vxBasePath + endpath;

    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "contents\\modules\\";
#else
    endpath = "contents/modules/";
#endif

    std::string path = vxBasePath + endpath;

    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "contents\\templates\\";
#else
    endpath = "contents/templates/";
#endif

    std::string path = vxBasePath + endpath;
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "contents\\plugins\\";
#else
    endpath = "contents/plugins/";
#endif

    std::string path = vxBasePath + endpath;
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "contents\\contents\\";
#else
    endpath = "contents/contents/";
#endif

    std::string path = vxBasePath + endpath;
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "contents\\assets\\";
#else
    endpath = "contents/assets/";
#endif

    std::string path = vxBasePath + endpath;
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    std::string endpath;
#ifdef _WIN32
    endpath = "sessions\\";
#else
    endpath = "sessions/";
#endif

    std::string path = vxBasePath + endpath;
    VortexMaker::createFolderIfNotExists(path);
  }

  {
    VortexMaker::createFolderIfNotExists(vortexProjectsPath);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "dists.json";

    nlohmann::json default_data = { { "vortexlauncher_dist", "stable" },
                                    { "vortex_dists", nlohmann::json::array({ "stable", "demo" }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "projects_pools.json";

    nlohmann::json default_data = { { "projects_pools", nlohmann::json::array({ vortexProjectsPath }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "modules_pools.json";

    std::string content_path;
#ifdef _WIN32
    content_path = "contents\\modules/";
#else
    content_path = "contents/modules/";
#endif

    nlohmann::json default_data = { { "modules_pools", nlohmann::json::array({ vxBasePath + content_path }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "plugins_pools.json";

    std::string content_path;
#ifdef _WIN32
    content_path = "contents\\plugins/";
#else
    content_path = "contents/plugins/";
#endif

    nlohmann::json default_data = { { "plugins_pools", nlohmann::json::array({ vxBasePath + content_path }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "templates_pools.json";

    std::string content_path;
#ifdef _WIN32
    content_path = "contents\\templates/";
#else
    content_path = "contents/templates/";
#endif

    nlohmann::json default_data = { { "templates_pools", nlohmann::json::array({ vxBasePath + content_path }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "contents_pools.json";

    std::string content_path;
#ifdef _WIN32
    content_path = "contents\\contents/";
#else
    content_path = "contents/contents/";
#endif

    nlohmann::json default_data = { { "contents_pools", nlohmann::json::array({ vxBasePath + content_path }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "configs/";
    std::string file = path + "vortex_versions_pools.json";

    std::string def_vx_path;

#if defined(_WIN32) || defined(_WIN64)
    def_vx_path = "C:/Program Files/Vortex";
#else
    def_vx_path = "/opt/Vortex/";
#endif
    nlohmann::json default_data = { { "vortex_versions_pools", nlohmann::json::array({ def_vx_path }) } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

  {
    std::string path = vxBasePath + "sessions/";
    std::string file = path + "active_sessions.json";

    nlohmann::json default_data = { { "sessions", nlohmann::json::array() } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }

{
    std::string path = vxBasePath + "contents/templates";
    std::string blank_template_path = path + "/blank_project";

#if defined(_WIN32) || defined(_WIN64)
    path = VortexMaker::convertPathToWindowsStyle(path);
    blank_template_path = VortexMaker::convertPathToWindowsStyle(blank_template_path);
#endif

    if (!fs::exists(blank_template_path)) {
        try {
            std::string cmd;

#ifdef _WIN32
            cmd = "xcopy \"" + VortexMaker::GetPath("resources/templates") + "\\blank_project\" \"" + path + "\\blank_project\\\" /E /I /Y /Q";
#else
            cmd = "cp -r \"" + VortexMaker::GetPath("resources/templates/blank_project") + "\" \"" + path + "\"";
#endif

            int res = VortexMaker::RunCommand(cmd.c_str());
            if (res != 0) {
                throw std::runtime_error("Copy command failed with exit code " + std::to_string(res));
            }
            VortexMaker::LogInfo("Core", "Path '" + blank_template_path + "' created with success.");
        } catch (const std::exception &ex) {
            VortexMaker::LogError("Core", "Error while creating folder '" + blank_template_path + "'");
            VortexMaker::LogError("Core", ex.what());
        }
    }
}


  {
    std::string path = vxBasePath + "data/";
    std::string file = path + "projects.json";

    nlohmann::json default_data = { { "projects", nlohmann::json::array() } };

    VortexMaker::createJsonFileIfNotExists(file, default_data);
  }
}
void VortexMaker::DetectPlatform() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
  ctx.platform = "windows";
#elif defined(__APPLE__) && defined(__MACH__)
  ctx.platform = "macos";
#elif defined(__linux__)
  ctx.platform = "linux";
#elif defined(__FreeBSD__)
  ctx.platform = "freebsd";
#else
  ctx.platform = "unknown";
#endif
}

void VortexMaker::DetectArch() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

#if defined(__x86_64__) || defined(_M_X64)
  ctx.arch = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
  ctx.arch = "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
  ctx.arch = "arm64";
#elif defined(__arm__) || defined(_M_ARM)
  ctx.arch = "arm";
#elif defined(__riscv)
  ctx.arch = "riscv";
#elif defined(__ppc64__)
  ctx.arch = "ppc64";
#else
  ctx.arch = "unknown";
#endif
}

std::chrono::system_clock::time_point addTimeoutToTime(const std::string &time_str, const std::string &timeout) {
  std::tm tm = {};
  std::stringstream ss(time_str);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

  auto time = std::chrono::system_clock::from_time_t(std::mktime(&tm));

  int number = std::stoi(timeout.substr(0, timeout.size() - 1));
  char unit = timeout.back();

  if (unit == 'h')
    return time + std::chrono::hours(number);
  else if (unit == 'd')
    return time + std::chrono::hours(number * 24);
  else if (unit == 'm')
    return time + std::chrono::hours(number * 24 * 30);
  else if (unit == 'y')
    return time + std::chrono::hours(number * 24 * 365);

  return time;
}

VORTEX_API void VortexMaker::UpdateSessions() {
  std::string homeDir = VortexMaker::getHomeDirectory();
  std::string sessions_dir;
  std::string config_path;

  if (VortexMaker::IsWindows()) {
    sessions_dir = homeDir + "\\.vx\\sessions";
    config_path = homeDir + "\\.vx\\configs\\sessions.json";
  } else
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

  {
    sessions_dir = homeDir + "/.vx/sessions";
    config_path = homeDir + "/.vx/configs/sessions.json";
  }

  std::ifstream config_file(config_path);
  nlohmann::json config;
  if (!config_file.is_open()) {
    std::cerr << "Error." << std::endl;
    return;
  }
  config_file >> config;
  config_file.close();

  std::string timeout = config["KeepSessionDataTimeout"];

  for (const auto &entry : fs::directory_iterator(sessions_dir)) {
    if (fs::is_directory(entry)) {
      std::string session_path = entry.path().string();
      std::string session_json_path;

      if (VortexMaker::IsWindows()) {
        session_json_path = session_path + "\\session.json";
      } else {
        session_json_path = session_path + "/session.json";
      }

      std::ifstream session_file(session_json_path);
      if (session_file.is_open()) {
        nlohmann::json session_data;
        session_file >> session_data;
        session_file.close();

        if (session_data.contains("SessionEndedAt")) {
          std::string session_ended_at = session_data["SessionEndedAt"];

          auto expiration_time = addTimeoutToTime(session_ended_at, timeout);
          auto now = std::chrono::system_clock::now();

          if (now > expiration_time) {
            fs::remove_all(session_path);
          }
        }
      } else {
        std::cerr << "Error : " << session_json_path << std::endl;
      }
    }
  }
}

VORTEX_API void VortexMaker::RefreshVortexDists() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\dists.json";
  } else {
    json_file = path + "/dists.json";
  }

  // Clear project list
  ctx.IO.sys_vortex_dists.clear();

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto dist : json_data["vortex_dists"]) {
      ctx.IO.sys_vortex_dists.push_back(dist);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::RefreshVortexLauncherDists() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\dists.json";
  } else {
    json_file = path + "/dists.json";
  }

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    ctx.IO.sys_vortexlauncher_dist = json_data["vortexlauncher_dist"].get<std::string>();
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::RefreshEnvironmentProjectsPools() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\projects_pools.json";
  } else {
    json_file = path + "/projects_pools.json";
  }

  // Clear project list
  ctx.IO.sys_projects_pools.clear();

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto pool : json_data["projects_pools"]) {
      ctx.IO.sys_projects_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::RefreshEnvironmentModulesPools() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\modules_pools.json";
  } else {
    json_file = path + "/modules_pools.json";
  }

  // Clear modules list
  ctx.IO.sys_modules_pools.clear();

  // Verify if the project is valid
  try {
    // Load JSON data from the modules configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto pool : json_data["modules_pools"]) {
      ctx.IO.sys_modules_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::RefreshEnvironmentContentsPools() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\contents_pools.json";
  } else {
    json_file = path + "/contents_pools.json";
  }

  // Clear contents list
  ctx.IO.sys_contents_pools.clear();

  // Verify if the contents pools file is valid
  try {
    // Load JSON data from the contents configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto pool : json_data["contents_pools"]) {
      ctx.IO.sys_contents_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::RefreshEnvironmentTemplatesPools() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\templates_pools.json";
  } else {
    json_file = path + "/templates_pools.json";
  }

  // Clear templates list
  ctx.IO.sys_templates_pools.clear();

  // Verify if the templates pools file is valid
  try {
    // Load JSON data from the templates configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto pool : json_data["templates_pools"]) {
      ctx.IO.sys_templates_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::RefreshEnvironmentPluginsPools() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\plugins_pools.json";
  } else {
    json_file = path + "/plugins_pools.json";
  }

  // Clear plugins list
  ctx.IO.sys_plugins_pools.clear();

  // Verify if the plugins pools file is valid
  try {
    // Load JSON data from the plugins configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto pool : json_data["plugins_pools"]) {
      ctx.IO.sys_plugins_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API bool VortexMaker::TestVortexExecutable(const std::string &path) {
  std::array<char, 128> buffer;
  std::string result;
  std::string command = path + " -test";

  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
  if (!pipe) {
    std::cerr << "popen() failed!" << std::endl;
    return false;
  }

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  int return_code = pclose(pipe.release());

  return (result.find("ok") != std::string::npos) && (return_code == 0);
}

VORTEX_API bool VortexMaker::CheckIfVortexVersionUtilityExist(const std::string &version, std::string &path) {
  VxContext &ctx = *CVortexMaker;

  size_t pos = version.find('.');
  std::string majorMinor = version;
  if (pos != std::string::npos) {
    pos = version.find('.', pos + 1);
    if (pos != std::string::npos) {
      majorMinor = version.substr(0, pos);
    }
  }

  for (auto &ver : ctx.IO.sys_vortex_versions) {
    path = ver->path;
    std::string contextVersion = ver->version;
    pos = contextVersion.find('.');
    if (pos != std::string::npos) {
      pos = contextVersion.find('.', pos + 1);
      if (pos != std::string::npos) {
        contextVersion = contextVersion.substr(0, pos);
      }
    }

    if (contextVersion == majorMinor) {
      return true;
    }
  }
  return false;
}

VORTEX_API void VortexMaker::RefreshEnvironmentVortexVersion() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;
  // ctx.IO.available_vortex_versions.clear();
  ctx.IO.sys_vortex_versions.clear();

  for (auto &base_path : VortexMaker::GetCurrentContext()->IO.sys_vortex_versions_pools) {
    if (!std::filesystem::exists(base_path)) {
      continue;
    }

    for (const auto &entry : std::filesystem::directory_iterator(base_path)) {
      if (entry.is_directory()) {
        std::string version_dir = entry.path().filename().string();
        std::string manifest_path = entry.path().string() + "/manifest.json";
        std::string vortex_executable = entry.path().string() + "/bin/vortex";

        if (std::filesystem::exists(manifest_path)) {
          try {
            std::ifstream manifest_file(manifest_path);
            nlohmann::json manifest_json;
            manifest_file >> manifest_json;

            std::string version = manifest_json["version"];
            std::string version_name = manifest_json["name"];

            std::string image_path = base_path + "/" + version_dir + "/" + manifest_json["image"].get<std::string>();
            if (!std::filesystem::exists(image_path)) {
              continue;
            }

            bool is_working = VortexMaker::TestVortexExecutable(vortex_executable);

            auto vortex_version = std::make_shared<VortexVersion>();
            vortex_version->version = version;
            vortex_version->name = version_name;
            vortex_version->banner = image_path;
            vortex_version->path = entry.path().string();
            vortex_version->working = is_working;

            ctx.IO.sys_vortex_versions.push_back(vortex_version);
            // ctx.IO.available_vortex_versions.push_back(version);
          } catch (const std::exception &e) {
            std::cout << "FAIL" << std::endl;
            continue;
          }
        } else {
          continue;
        }
      }
    }
  }
}

VORTEX_API void VortexMaker::RefreshEnvironmentVortexVersionsPools() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\configs\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
  }

  std::string json_file;
  if (VortexMaker::IsWindows()) {
    json_file = path + "\\vortex_versions_pools.json";
  } else {
    json_file = path + "/vortex_versions_pools.json";
  }

  // Clear vortex versions list
  ctx.IO.sys_vortex_versions_pools.clear();

  // Verify if the vortex versions file is valid
  try {
    // Load JSON data from the vortex versions configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    for (auto pool : json_data["vortex_versions_pools"]) {
      ctx.IO.sys_vortex_versions_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

void VortexMaker::RefreshEnvironmentProjects() {
  VxContext &ctx = *CVortexMaker;

  ctx.IO.sys_projects.clear();

  for (const auto &pool_path : ctx.IO.sys_projects_pools) {
    try {
      if (!std::filesystem::exists(pool_path)) {
        VortexMaker::LogError("Error: Pool path does not exist - ", pool_path.c_str());
        continue;
      }

      if (!std::filesystem::is_directory(pool_path)) {
        VortexMaker::LogError("Error: Pool path is not a directory - ", pool_path.c_str());
        continue;
      }

      for (const auto &entry : std::filesystem::recursive_directory_iterator(pool_path)) {
        if (entry.is_regular_file() && entry.path().filename() == "vortex.config") {
          try {
            std::cout << "Found vortex.config at: " << entry.path() << std::endl;

            std::ifstream file(entry.path());
            if (file.is_open()) {
              nlohmann::json json_data;
              file >> json_data;

              if (json_data.contains("project")) {
                nlohmann::json project_data = json_data["project"];
                auto project = std::make_shared<EnvProject>();

                project->name = project_data.value("name", "");
                project->path = entry.path().parent_path().string();
                project->version = project_data.value("version", "");
                project->compatibleWith = project_data.value("compatibleWith", "");
                project->description = project_data.value("description", "");

                std::string logo_path = project->path;

                // TODO : Proper function to replace / to \\ if win
                if (!logo_path.empty() && logo_path.back() != '/' && logo_path.back() != '\\') {
#ifdef _WIN32
                  logo_path += "\\";
#else
                  logo_path += "/";
#endif
                }
                logo_path += project_data.value("logoPath", "");

#ifdef _WIN32
                std::replace(logo_path.begin(), logo_path.end(), '/', '\\');
#endif

                if (std::filesystem::exists(logo_path)) {
                  project->logoPath = logo_path;
                }

                project->author = project_data.value("author", "");
                project->lastOpened = project_data.value("lastOpened", "unknown");

                std::cout << "Punt" << std::endl;
                ctx.IO.sys_projects.push_back(project);
              }
            } else {
              VortexMaker::LogError("Error: Failed to open file - ", entry.path().string());
            }
          } catch (const std::exception &e) {
            VortexMaker::LogError("Error reading config file: ", e.what());
          }
        }
      }
    } catch (const std::exception &e) {
      VortexMaker::LogError("Error accessing pool path: ", e.what());
    }
  }
}

VORTEX_API void VortexMaker::OpenFolderInFileManager(const std::string &path) {
#if defined(_WIN32) || defined(_WIN64)
  std::string command = "explorer \"" + path + "\"";
  VortexMaker::RunCommand(command.c_str());
#elif defined(__APPLE__)
  std::string command = "open \"" + path + "\"";
  VortexMaker::RunCommand(command.c_str());
#elif defined(__linux__)
  std::string command =
      "dbus-send --session "
      "--dest=org.freedesktop.FileManager1 "
      "--type=method_call "
      "/org/freedesktop/FileManager1 "
      "org.freedesktop.FileManager1.ShowFolders "
      "array:string:\"" +
      path + "\" string:\"\"";

  int retCode = VortexMaker::RunCommand(command.c_str());
  if (retCode != 0) {
    std::cerr << "D-Bus method failed, trying xdg-open...\n";
    command = "xdg-open \"" + path + "\"";
    VortexMaker::RunCommand(command.c_str());
  }
#else
#error "OS not supported!"
#endif
}

VORTEX_API void VortexMaker::UpdateEnvironmentProject(
    const std::string &name,
    const std::string &author,
    const std::string &version,
    const std::string &compatibleWith,
    const std::string &description,
    const std::string &path,
    const std::string &logo_path,
    const std::string &template_name) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string sys_path;
  if (VortexMaker::IsWindows()) {
    sys_path = VortexMaker::getHomeDirectory() + "\\.vx\\data\\";
  } else {
    sys_path = VortexMaker::getHomeDirectory() + "/.vx/data/";
  }

  std::string json_file = sys_path + "projects.json";

  // Verify if the project is valid
  // Load JSON data from the project configuration file
  auto json_data = VortexMaker::DumpJSON(json_file);

  VortexMaker::LogInfo("Core", "Verify if the new project \"" + name + "\" is not already existing.");

  // Check if a project with the given name exists
  bool projectExists = false;
  for (auto &project : json_data["projects"]) {
    if (project["name"].get<std::string>() == name) {
      projectExists = true;
      return;
    }
  }

  VortexMaker::LogInfo("Core", "Create a new entry in registered projects for \"" + name + "\".");

  // If the project doesn't exist, create a new JSON object and add it to the list
  json_data["projects"].push_back({ { "name", name },
                                    { "version", version },
                                    { "description", description },
                                    { "path", path },
                                    { "lastOpened", VortexMaker::getCurrentTimeStamp() },
                                    { "compatibleWith", compatibleWith },
                                    { "logoPath", logo_path } });

  // Write the updated JSON data back to the file
  std::ofstream output(json_file);
  output << json_data.dump(4);  // Use pretty print with indentation of 4 spaces
  output.close();
}

VORTEX_API void VortexMaker::InitializePlatformVendor() {
#if defined(__linux__)
  VortexMaker::GetCurrentContext()->m_PlatformVendor = PlatformVendor::Linux;
#elif defined(_WIN32) || defined(_WIN64)
  VortexMaker::GetCurrentContext()->m_PlatformVendor = PlatformVendor::Windows;
#elif defined(__APPLE__)
  VortexMaker::GetCurrentContext()->m_PlatformVendor = PlatformVendor::Macos;
#else
  //
#endif
}

VORTEX_API bool VortexMaker::IsLinux() {
  return VortexMaker::GetCurrentContext()->m_PlatformVendor == PlatformVendor::Linux;
}

VORTEX_API bool VortexMaker::IsNotLinux() {
  return !IsLinux();
}

VORTEX_API bool VortexMaker::IsWindows() {
  return VortexMaker::GetCurrentContext()->m_PlatformVendor == PlatformVendor::Windows;
}

VORTEX_API bool VortexMaker::IsNotWindows() {
  return !IsWindows();
}

VORTEX_API bool VortexMaker::IsMacOs() {
  return VortexMaker::GetCurrentContext()->m_PlatformVendor == PlatformVendor::Macos;
}

VORTEX_API bool VortexMaker::IsNotMacOS() {
  return !IsMacOs();
}

VORTEX_API void VortexMaker::UpdateEnvironmentProject() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\data\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/data/";
  }

  std::string json_file = path + "projects.json";

  try {
    // Load JSON data from the project configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);

    std::string name = ctx.name;

    bool projectExists = false;
    for (auto &project : json_data["projects"]) {
      if (project["name"].get<std::string>() == name) {
        // Project exists, update its information
        project["version"] = ctx.project_version;
        project["description"] = ctx.description;
        project["path"] = ctx.projectPath;
        project["compatibleWith"] = ctx.compatibleWith;
        project["lastOpened"] = VortexMaker::getCurrentTimeStamp();
        project["logoPath"] = ctx.logoPath;
        projectExists = true;
        break;
      }
    }

    // Create a new entry if project doesn't exist
    if (!projectExists) {
      json_data["projects"].push_back({ { "name", ctx.name },
                                        { "version", ctx.project_version },
                                        { "description", ctx.description },
                                        { "path", ctx.projectPath },
                                        { "lastOpened", VortexMaker::getCurrentTimeStamp() },
                                        { "compatibleWith", ctx.compatibleWith },
                                        { "logoPath", ctx.logoPath } });
    }

    // Save the updated JSON data back to the file
    std::ofstream output(json_file);
    output << json_data.dump(4);  // Pretty print with indentation
    output.close();
  } catch (const std::exception &e) {
    // Log error if any exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::UpdateEnvironmentProject(const std::string &oldname) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set path depending on platform
  std::string path;
  if (VortexMaker::IsWindows()) {
    path = VortexMaker::getHomeDirectory() + "\\.vx\\data\\";
  } else {
    path = VortexMaker::getHomeDirectory() + "/.vx/data/";
  }

  std::string json_file = path + "projects.json";

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);

    // Check if a project with the old name exists
    bool projectExists = false;
    for (auto &project : json_data["projects"]) {
      if (project["name"].get<std::string>() == oldname) {
        // Project with the old name exists, update its information
        project["name"] = ctx.name;
        project["version"] = ctx.version;
        project["description"] = ctx.description;
        project["path"] = ctx.projectPath;
        project["logoPath"] = ctx.logoPath;
        projectExists = true;
        break;
      }
    }

    // If the project doesn't exist, create a new JSON object and add it to the list
    if (!projectExists) {
      json_data["projects"].push_back({ { "name", ctx.name },
                                        { "version", ctx.version },
                                        { "description", ctx.description },
                                        { "path", ctx.projectPath },
                                        { "logoPath", ctx.logoPath } });
    }

    // Write the updated JSON data back to the file
    std::ofstream output(json_file);
    output << json_data.dump(4);  // Pretty print with indentation of 4 spaces
    output.close();
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    VortexMaker::LogError("Error: ", e.what());
  }
}

VORTEX_API void VortexMaker::PostLatestVortexVersion(const std::shared_ptr<VortexVersion> &version) {
  VxContext &ctx = *CVortexMaker;

  std::string path = VortexMaker::getHomeDirectory() + (VortexMaker::IsWindows() ? "\\.vx\\data\\" : "/.vx/data/");
  std::string json_file = path + (VortexMaker::IsWindows() ? "launcher_data.json" : "launcher_data.json");

  nlohmann::json default_data = { { "latest_saved_version", nlohmann::json::array() } };

  VortexMaker::createJsonFileIfNotExists(json_file, default_data);

  std::ifstream file_in(json_file);
  nlohmann::json data;
  if (file_in.is_open()) {
    file_in >> data;
    file_in.close();
  }

  nlohmann::json version_data = {
    { "version", ctx.latest_vortex_version->version }, { "name", ctx.latest_vortex_version->name },
    { "path", ctx.latest_vortex_version->path },       { "sum", ctx.latest_vortex_version->sum },
    { "date", ctx.latest_vortex_version->date },       { "banner", ctx.latest_vortex_version->banner }
  };

  data["latest_saved_version"] = version_data;

  std::ofstream file_out(json_file);
  if (file_out.is_open()) {
    file_out << data.dump(4);
    file_out.close();
  }
}

VORTEX_API VortexVersion VortexMaker::CheckLatestVortexVersion() {
  VxContext &ctx = *CVortexMaker;

  std::string path = VortexMaker::getHomeDirectory() + (VortexMaker::IsWindows() ? "\\.vx\\data\\" : "/.vx/data/");
  std::string json_file = path + (VortexMaker::IsWindows() ? "launcher_data.json" : "launcher_data.json");

  VortexVersion latest_version;

  std::ifstream file_in(json_file);
  if (file_in.is_open()) {
    nlohmann::json data;
    file_in >> data;
    file_in.close();

    if (data.contains("latest_saved_version")) {
      latest_version.version = data["latest_saved_version"].value("version", "");
      latest_version.name = data["latest_saved_version"].value("name", "");
      latest_version.path = data["latest_saved_version"].value("path", "");
      latest_version.sum = data["latest_saved_version"].value("sum", "");
      latest_version.date = data["latest_saved_version"].value("date", "");
      latest_version.banner = data["latest_saved_version"].value("banner", "");
    }
  }

  return latest_version;
}
