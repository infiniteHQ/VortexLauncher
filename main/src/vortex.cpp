// The VortexMaker Project
// [Main sources]

// Index of this file:

//-------------------------------------------------------------------------
// [INCLUDES]
//-------------------------------------------------------------------------

#include "../include/vortex.h"
#ifndef VORTEX_DISABLE
#include "../include/vortex_internals.h"

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------
// Current runtime pointer.
#ifndef CVortexMaker
VxContext *CVortexMaker = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// - You probably don't want to modify that mid-program, and if you use
// global/static e.g. ImVector<> instances you may need to keep them accessible
// during program destruction.
// - DLL users: read comments above.

#ifndef DISABLE_DEFAULT_ALLOCATORS

/**
 * @brief Wrapper function for malloc.
 *
 * This function is a wrapper for the malloc function.
 * It allocates memory of the specified size using the standard malloc function.
 *
 * @param size The size of memory to allocate.
 * @param user_data Unused user data pointer (required by Vortex allocator signature).
 * @return A pointer to the allocated memory, or nullptr if allocation fails.
 */
static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(user_data);  // Unused parameter
  return malloc(size);   // Call standard malloc
}

/**
 * @brief Wrapper function for free.
 *
 * This function is a wrapper for the free function.
 * It deallocates memory pointed to by ptr using the standard free function.
 *
 * @param ptr Pointer to the memory block to deallocate.
 * @param user_data Unused user data pointer (required by Vortex allocator signature).
 */
static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(user_data);  // Unused parameter
  free(ptr);             // Call standard free
}

#else  // DISABLE_DEFAULT_ALLOCATORS

/**
 * @brief Dummy MallocWrapper function.
 *
 * This function is a dummy wrapper for malloc used when default allocators are disabled.
 * It should not be called and will trigger an assertion.
 *
 * @param size Unused size parameter.
 * @param user_data Unused user data parameter.
 * @return Always returns nullptr, triggering an assertion.
 */
static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(size);       // Unused parameter
  VX_UNUSED(user_data);  // Unused parameter
  VX_ASSERT(0);          // Trigger assertion, should not be called
  return nullptr;        // Return nullptr
}

/**
 * @brief Dummy FreeWrapper function.
 *
 * This function is a dummy wrapper for free used when default allocators are disabled.
 * It should not be called and will trigger an assertion.
 *
 * @param ptr Unused pointer parameter.
 * @param user_data Unused user data parameter.
 */
static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(ptr);        // Unused parameter
  VX_UNUSED(user_data);  // Unused parameter
  VX_ASSERT(0);          // Trigger assertion, should not be called
}

#endif  // DISABLE_DEFAULT_ALLOCATORS

static VortexMakerMemAllocFunc CVxAllocatorAllocFunc = MallocWrapper;
static VortexMakerMemFreeFunc CVxAllocatorFreeFunc = FreeWrapper;
static void *CVxAllocatorUserData = NULL;

/**
 * @brief CreateContext creates a new Vortex context.
 *
 * This function creates a new Vortex context and initializes it. It ensures memory safety
 * by properly managing memory allocation and context switching.
 *
 * @return A pointer to the newly created Vortex context.
 */
VORTEX_API VxContext *VortexMaker::CreateContext() {
  // Save the previous context before creating a new one
  VxContext *prev_ctx = GetCurrentContext();

  // Allocate memory for the new context
  VxContext *ctx = VX_NEW(VxContext);

  // Set the current context to the newly created context
  SetCurrentContext(ctx);

  // Restore the previous context if it exists
  if (prev_ctx != nullptr)
    SetCurrentContext(prev_ctx);

  // Return the pointer to the newly created context
  return ctx;
}

/**
 * @brief SetCurrentContext sets the current Vortex context.
 *
 * This function sets the current Vortex context to the provided context pointer.
 * It offers flexibility by allowing custom thread-based context control if enabled.
 *
 * @param ctx A pointer to the Vortex context to be set as the current context.
 */
void VortexMaker::SetCurrentContext(VxContext *ctx) {
#ifdef USE_CURRENT_CONTEXT_FUNC
  // If custom thread-based control is enabled, call the custom function
  USE_CURRENT_CONTEXT_FUNC(ctx);
#else
  // Otherwise, set the current context directly
  CVortexMaker = ctx;
#endif
}

/**
 * @brief DestroyContext destroys a Vortex context.
 *
 * This function destroys the specified Vortex context. If no context is provided,
 * it destroys the current context. It ensures proper context switching and memory deallocation.
 *
 * @param ctx A pointer to the Vortex context to be destroyed. If nullptr, the current context will be destroyed.
 */
VORTEX_API void VortexMaker::DestroyContext(VxContext *ctx) {
  // Save the previous context before destroying the specified context
  VxContext *prev_ctx = GetCurrentContext();

  // If no context is provided, destroy the current context
  if (ctx == nullptr)
    ctx = prev_ctx;

  // Set the current context to the one to be destroyed
  SetCurrentContext(ctx);

  // If the previous context is different from the one to be destroyed, restore it
  SetCurrentContext((prev_ctx != ctx) ? prev_ctx : nullptr);

  // Deallocate memory for the context
  VX_DELETE(ctx);
}

VORTEX_API std::string VortexMaker::getHomeDirectory() {
  if (VortexMaker::IsLinux() || VortexMaker::IsMacOs()) {
    const char *homePath = std::getenv("HOME");
    if (homePath == nullptr) {
      throw std::runtime_error("HOME environment variable not set");
    }
    return std::string(homePath);
  } else if (VortexMaker::IsWindows()) {
    const char *homePath = std::getenv("USERPROFILE");
    if (homePath == nullptr) {
      const char *homeDrive = std::getenv("HOMEDRIVE");
      const char *homePathEnv = std::getenv("HOMEPATH");
      if (homeDrive == nullptr || homePathEnv == nullptr) {
        throw std::runtime_error("HOMEPATH environment variables not set");
      }
      return std::string(homeDrive) + std::string(homePathEnv);
    }
    return std::string(homePath);
  }

  throw std::runtime_error("Unknown platform: Unable to determine home directory");
}

void initialize_random() {
  srand(static_cast<unsigned int>(time(0)));
}

std::string generateSessionID() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  char buffer[64];
  std::strftime(buffer, sizeof(buffer), "editor-%m-%d-%Y-%H-%M-%S", &tm);

  int random_number = rand() % 10000;
  return std::string(buffer) + "-" + std::to_string(random_number);
}

void addSessionToJson(
    const std::string &session_id,
    const std::string &version,
    const std::string &user,
    const std::string &path) {
  // Set path depending on platform
  std::string json_path;
  if (VortexMaker::IsWindows()) {
    json_path = VortexMaker::getHomeDirectory() + "\\.vx\\sessions\\active_sessions.json";
  } else {
    json_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/active_sessions.json";
  }

  std::ifstream file_in(json_path);
  nlohmann::json active_sessions;

  // Load existing active sessions from the JSON file
  if (file_in.is_open()) {
    file_in >> active_sessions;
    file_in.close();
  }

  // Create a new session entry
  nlohmann::json new_session = { { "session_path", path },
                                 { "session_id", session_id },
                                 { "session_start_at", std::time(nullptr) },
                                 { "session_owner", user },
                                 { "vortex_version", version },
                                 { "vortex_exec_path", "/opt/Vortex/" + version + "/bin/vortex" } };

  // Add the new session to the sessions array
  active_sessions["sessions"].push_back(new_session);

  // Save the updated sessions back to the JSON file
  std::ofstream file_out(json_path);
  file_out << active_sessions.dump(4);  // Pretty print with indentation
}

// TODO : In editor (if the launcher is stopped before the editor.)
void removeSessionFromJson(const std::string &session_id) {
  // Set path depending on platform
  std::string json_path;
  if (VortexMaker::IsWindows()) {
    json_path = VortexMaker::getHomeDirectory() + "\\.vx\\sessions\\active_sessions.json";
  } else {
    json_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/active_sessions.json";
  }

  std::ifstream file_in(json_path);
  nlohmann::json active_sessions;

  // Load existing active sessions from the JSON file
  if (file_in.is_open()) {
    file_in >> active_sessions;
    file_in.close();
  }

  // Find and remove the session with the given session_id
  for (auto it = active_sessions["sessions"].begin(); it != active_sessions["sessions"].end(); ++it) {
    if ((*it)["session_id"] == session_id) {
      active_sessions["sessions"].erase(it);
      break;  // Exit loop after removing the session
    }
  }

  // Save the updated sessions back to the JSON file
  std::ofstream file_out(json_path);
  file_out << active_sessions.dump(4);  // Pretty print with indentation
}

VORTEX_API bool VortexMaker::CheckIfProjectRunning(const std::string &path) {
  // Set path depending on platform
  std::string json_path;
  if (VortexMaker::IsWindows()) {
    json_path = VortexMaker::getHomeDirectory() + "\\.vx\\sessions\\active_sessions.json";
  } else {
    json_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/active_sessions.json";
  }

  std::ifstream file_in(json_path);
  nlohmann::json active_sessions;

  // Load existing active sessions from the JSON file
  if (file_in.is_open()) {
    file_in >> active_sessions;
    file_in.close();
  } else {
    std::cerr << "Fatal: Could not open the active sessions file." << std::endl;
    return false;
  }

  // Check if the session with the given path is currently running
  for (const auto &session : active_sessions["sessions"]) {
    if (session.contains("session_path") && session["session_path"] == path) {
      return true;  // Project is running
    }
  }

  return false;  // Project is not running
}

void writeSessionEndState(const std::string &session_id, const std::string &state) {
  // Set the session directory path depending on the platform
  std::string session_dir;
  if (VortexMaker::IsWindows()) {
    session_dir = VortexMaker::getHomeDirectory() + "\\.vx\\sessions\\" + session_id;
  } else {
    session_dir = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + session_id;
  }

  std::string session_file_path = session_dir + "/session.json";

  // Get current time in ISO 8601 format
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&now_time);
  char buffer[32];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm);

  nlohmann::json session_data = { { "SessionID", session_id },
                                  { "SessionEndedAt", buffer },
                                  { "SessionEndedWithState", state } };

  // Create the session directory if it does not exist
  std::filesystem::create_directories(session_dir);

  // Write session data to the JSON file
  std::ofstream session_file(session_file_path);
  if (session_file.is_open()) {
    session_file << session_data.dump(4);  // Pretty print with indentation
    session_file.close();
  } else {
    std::cerr << "Error: Could not open session file for writing." << std::endl;
  }
}

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

std::string escapeQuotes(const std::string &command) {
  std::string escapedCommand;
  for (char c : command) {
    if (c == '"') {
      escapedCommand += "\\\"";
    } else {
      escapedCommand += c;
    }
  }
  return escapedCommand;
}

bool VortexMaker::executeInChildProcess(const std::string &command) {
#ifdef _WIN32
  STARTUPINFOA si = { 0 };
  PROCESS_INFORMATION pi = { 0 };
  si.cb = sizeof(STARTUPINFOA);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;

  char commandLine[4096];
  strncpy(commandLine, command.c_str(), sizeof(commandLine) - 1);
  commandLine[sizeof(commandLine) - 1] = '\0';

  if (!CreateProcessA(
          NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
    std::cerr << "Error while creating process for command: " << command << " Error: " << GetLastError() << std::endl;
    return false;
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return true;
#else
  pid_t pid = fork();

  if (pid == -1) {
    std::cerr << "Error while forking" << std::endl;
    return false;
  }

  if (pid == 0) {
    // In child process
    std::vector<std::string> args = { "bash", "-c", command };

    std::vector<char *> cArgs;
    for (auto &arg : args) {
      cArgs.push_back(&arg[0]);
    }
    cArgs.push_back(nullptr);

    execvp(cArgs[0], cArgs.data());

    std::cerr << "Error while executing command: " << args[0] << std::endl;
    _exit(1);
  }

  // In parent process
  int status;
  waitpid(pid, &status, 0);

  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    return true;
  } else {
    std::cerr << "Child process failed with status: " << WEXITSTATUS(status) << std::endl;
    return false;
  }
#endif
}

std::string convertPathToWindowsStyle(const std::string &path) {
  std::string windowsPath = path;
  std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
  return windowsPath;
}

// TODO: Open by Vortex version shared_ptr, not by version (avoid version collision)
VORTEX_API void VortexMaker::OpenProject(const std::string &path, const std::string &version) {
  std::string session_id = generateSessionID();
  addSessionToJson(session_id, version, "user", path);

  std::string project_path = path;
  std::string vortex_path;
  for (auto vortex : VortexMaker::GetCurrentContext()->IO.sys_vortex_versions) {
    if (vortex->version == version) {
      vortex_path = vortex->path;
    }
  }

  if (VortexMaker::IsWindows()) {
    project_path = convertPathToWindowsStyle(project_path);
    vortex_path = convertPathToWindowsStyle(vortex_path);
  }

  std::string command;
  if (VortexMaker::IsWindows()) {
    command = "cmd.exe /C \"" + vortex_path + "\\bin\\vortex.exe\" --editor --session_id=" + "\"" + session_id + "\"" + "\"";
  } else {
    command = "\"" + vortex_path + "/bin/vortex\" --editor --session_id=" + "\"" + session_id + "\"";
  }

  std::string target_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + session_id + "/crash/core_dumped.txt";
  std::string crash_script_command;

  if (VortexMaker::IsWindows()) {
    crash_script_command = "cmd.exe /C \"cd \"" + project_path + "\" && \"" + vortex_path + "\\bin\\handle_crash.bat\" \"" +
                           target_path + "\"; & \"" + vortex_path + "\\bin\\vortex.exe\" --editor --session_id=" + "\"" +
                           session_id + "\"" + "; & \"" + vortex_path + "\\bin\\vortex.exe\" -crash --session_id=" + "\"" +
                           session_id + "\"" + " ; & \"" + vortex_path +
                           "\\bin\\vortex_utils.exe\" -rms --session_id=" + "\"" + session_id + "\"" + "\"";
  } else {
    crash_script_command =
        "cd \"" + project_path + "\" && bash \"" + vortex_path + "/bin/handle_crash.sh\" " + target_path + " " + command;
  }

  std::cout << "Bootstrap: Starting with command: " << crash_script_command << std::endl;

  if (VortexMaker::executeInChildProcess(crash_script_command)) {
    writeSessionEndState(session_id, "success");
  } else {
    std::string crash_handle_command;
    if (!VortexMaker::IsWindows()) {
      crash_handle_command = "\"" + vortex_path + "/bin/vortex\" -crash --session_id=" + session_id;
      VortexMaker::executeInChildProcess(crash_handle_command);
      writeSessionEndState(session_id, "fail");
    }
  }

  // TODO in crash handler or finish
  // removeSessionFromJson(session_id);
}

std::string escapeSpaces(const std::string &input) {
  std::string escaped;
  for (char c : input) {
    if (c == ' ') {
      escaped += "\\\\ ";
    } else {
      escaped += c;
    }
  }
  return escaped;
}

VORTEX_API void VortexMaker::OpenVortexUninstaller(const std::string &path) {
  VxContext &ctx = *CVortexMaker;

  std::string command;

  std::string escapedPath = escapeSpaces(path);

  if (VortexMaker::IsWindows()) {
    command = ctx.m_VortexLauncherPath + "\\VersionUninstaller.exe --path=" + escapedPath;
  } else {
    command = ctx.m_VortexLauncherPath + "/VersionUninstaller --path=" + escapedPath;
  }

  bool success = VortexMaker::executeInChildProcess(command);

  if (success) {
    std::cout << "Uninstallation succeeded." << std::endl;
  } else {
    std::cerr << "Uninstallation failed." << std::endl;
  }
}

VORTEX_API void VortexMaker::OpenVortexInstaller(
    const std::string &version,
    const std::string &arch,
    const std::string &dist,
    const std::string &platform) {
  VxContext &ctx = *CVortexMaker;

  std::string command;

  if (VortexMaker::IsWindows()) {
    command = ctx.m_VortexLauncherPath + "\\VersionInstaller.exe --home=" + VortexMaker::getHomeDirectory() +
              " --dist=" + dist + " --platform=" + platform + " --arch=" + arch + " --version=" + version;
  } else {
    command = ctx.m_VortexLauncherPath + "/VersionInstaller --home=" + VortexMaker::getHomeDirectory() + " --dist=" + dist +
              " --platform=" + platform + " --arch=" + arch + " --version=" + version;
  }

  VXWARN("F", command)

  bool success = VortexMaker::executeInChildProcess(command);

  if (success) {
    std::cout << "Installation succeeded." << std::endl;
  } else {
    std::cerr << "Installation failed." << std::endl;
  }
}

VORTEX_API void VortexMaker::OpenLauncherUpdater(const std::string &path, const std::string &dist) {
  VxContext &ctx = *CVortexMaker;

  std::string command;

#ifdef _WIN32
  command = ctx.m_VortexLauncherPath + "\\VortexUpdater.exe --path=" + path + " --workdir=" + path + " --dist=" + dist;
#else
  command = ctx.m_VortexLauncherPath + "/VortexUpdater --path=" + path + " --workdir=" + path + " --dist=" + dist;
#endif

  std::cout << "CMLD : " << command << std::endl;

  bool success = executeInChildProcess(command);

  if (success) {
    std::cout << "Launcher update succeeded." << std::endl;
  } else {
    std::cerr << "Launcher update failed." << std::endl;
  }
}

/**
 * @brief Set custom allocator functions and user data for VortexMaker.
 *
 * This function allows setting custom allocator functions and user data
 * for VortexMaker. These custom functions and data will be used by VortexMaker
 * for memory allocation and deallocation.
 *
 * @param alloc_func The custom allocator function pointer.
 * @param free_func The custom free function pointer.
 * @param user_data The custom user data pointer.
 */
void VortexMaker::SetAllocatorFunctions(
    VortexMakerMemAllocFunc alloc_func,
    VortexMakerMemFreeFunc free_func,
    void *user_data) {
  // Set the custom allocator functions and user data for VortexMaker
  CVxAllocatorAllocFunc = alloc_func;
  CVxAllocatorFreeFunc = free_func;
  CVxAllocatorUserData = user_data;
}

/**
 * @brief Retrieve allocator functions and user data.
 *
 * This function is provided to facilitate copying allocators from one static/DLL
 * boundary to another. It retrieves the allocator functions and user data
 * used by the VortexMaker.
 *
 * @param p_alloc_func Pointer to store the allocator function.
 * @param p_free_func Pointer to store the free function.
 * @param p_user_data Pointer to store the user data.
 */
void VortexMaker::GetAllocatorFunctions(
    VortexMakerMemAllocFunc *p_alloc_func,
    VortexMakerMemFreeFunc *p_free_func,
    void **p_user_data) {
  // Retrieve the allocator functions and user data from the VortexMaker
  *p_alloc_func = CVxAllocatorAllocFunc;
  *p_free_func = CVxAllocatorFreeFunc;
  *p_user_data = CVxAllocatorUserData;
}

#ifdef _WIN32
std::string VortexMaker::convertPathToWindowsStyle(const std::string &path) {
  std::string windowsPath = path;
  std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
  return windowsPath;
}
#endif

std::string VortexMaker::CookPath(std::string_view input_path) {
  static const std::string root_path = []() {
    std::string path;
#ifdef _WIN32
    char result[MAX_PATH];
    if (GetModuleFileNameA(NULL, result, MAX_PATH))
      path = std::filesystem::path(result).parent_path().string();
    else
      std::cerr << "Failed to get root path" << std::endl;
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, sizeof(result) - 1);
    if (count != -1) {
      result[count] = '\0';
      path = std::filesystem::path(result).parent_path().string();
    } else {
      std::cerr << "Failed to get root path" << std::endl;
    }
#endif
    return path;
  }();

  return (input_path.empty() || input_path[0] == '/') ? std::string(input_path) : root_path + "/" + std::string(input_path);
}

std::string VortexMaker::GetPath(const std::string &path) {
#ifdef _WIN32
  return VortexMaker::convertPathToWindowsStyle(VortexMaker::CookPath(path));
#else
  return VortexMaker::CookPath(path);
#endif
}


#ifdef _WIN32
#include <windows.h>
#include <string>

int VortexMaker::RunCommand(const std::string& command) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    std::string cmdLine = "cmd.exe /C " + command;

    BOOL success = CreateProcessA(
        NULL,
        cmdLine.data(),
        NULL, NULL, FALSE,
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi
    );

    if (!success) {
        return 1; 
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return static_cast<int>(exitCode);
}
#else
#include <cstdlib>

int VortexMaker::RunCommand(const std::string& command) {
    return std::system(command.c_str());
}
#endif

VORTEX_API void VortexMaker::InstallPluginToSystem(const std::string &path, const std::string &pool_path) {
  std::string plugins_path = pool_path;
  std::string json_file = path + "/plugin.json";

  // Verify if the plugin is valid
  try {
    // Load JSON data from the plugin configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    plugins_path += "/" + name + "." + version;  // TODO : HASH name to avoid collision

    VortexMaker::LogInfo("Core", "Installing the plugin " + name + "...");

    // Create directory
    {
      std::string cmd = "mkdir " + plugins_path;
      system(cmd.c_str());
    }

    // Move the plugin in the final system path
    {
      std::string cmd = "cp -r " + path + "/* " + plugins_path;
      system(cmd.c_str());
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

VORTEX_API void VortexMaker::InstallModuleToSystem(const std::string &path, const std::string &pool_path) {
  std::string modules_path = pool_path;
  std::string json_file = path + "/module.json";

  // Verify if the module is valid
  try {
    // Load JSON data from the module configuration file
    auto json_data = VortexMaker::DumpJSON(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    modules_path += "/" + name + "." + version;  // TODO : HASH name to avoid collision

    VortexMaker::LogInfo("Core", "Installing the module " + name + "...");

    // Create directory
    {
      std::string cmd = "mkdir " + modules_path;
      system(cmd.c_str());
    }

    // Move the module in the final system path
    {
      std::string cmd = "cp -r " + path + "/* " + modules_path;
      system(cmd.c_str());
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

VortexVersion VortexMaker::CheckVersionAvailibility(const std::string &version) {
  VxContext &ctx = *CVortexMaker;

  for (auto &available_version : ctx.latest_vortex_versions) {
    if (available_version.version == version) {
      return available_version;
    }
  }
  return VortexVersion();
}

VORTEX_API void VortexMaker::OpenURL(const std::string &url) {
#if defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
  std::string command = "open " + url;
  system(command.c_str());
#elif defined(__linux__)
  std::string command = "xdg-open " + url;
  system(command.c_str());
#else
  std::cerr << "Unsupported platform: unable to open URL." << std::endl;
#endif
}

/**
 * @brief Get the current Vortex context.
 *
 * This function returns a pointer to the current Vortex context.
 * It simply returns the value of the CVortexMaker pointer.
 *
 * @return A pointer to the current Vortex context.
 */
VORTEX_API VxContext *VortexMaker::GetCurrentContext() {
  return CVortexMaker;  // Return the current Vortex context pointer
}

/**
 * @brief Allocate memory using the Vortex memory allocator.
 *
 * This function allocates memory of the specified size using the Vortex memory allocator.
 * It calls the CVxAllocatorAllocFunc function pointer with the provided size and allocator user data.
 * If IMGUI debug tools are enabled, it also calls DebugAllocHook to handle debug allocation information.
 *
 * @param size The size of memory to allocate.
 * @return A pointer to the allocated memory, or nullptr if allocation fails.
 */
void *VortexMaker::MemAlloc(size_t size) {
  // Call the Vortex memory allocator function pointer to allocate memory
  void *ptr = (*CVxAllocatorAllocFunc)(size, CVxAllocatorUserData);

#ifndef VX_DISABLE_DEBUG_TOOLS
  // Check if IMGUI debug tools are enabled and CVortexMaker is valid
  if (VxContext *ctx = CVortexMaker) {
    // Call the debug allocation hook to handle debug information
    DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
  }
#endif

  return ptr;  // Return the allocated memory pointer
}

// Vx_FREE() == VortexMaker::MemFree()
void VortexMaker::MemFree(void *ptr) {
  if (ptr)
    if (VxContext *ctx = CVortexMaker)
      ctx->IO.MetricsActiveAllocations--;
  return (*CVxAllocatorFreeFunc)(ptr, CVxAllocatorUserData);
}

// We record the number of allocation in recent frames, as a way to
// audit/sanitize our guiding principles of "no allocations on idle/repeating
// frames"
void VortexMaker::DebugAllocHook(VortexMakerDebugAllocInfo *info, void *ptr, size_t size) {
  // VortexMakerDebugAllocEntry* entry = &info->LastEntriesBuf[info->LastEntriesIdx];
  VX_UNUSED(ptr);

  /*
  if (entry->FrameCount != frame_count)
  {
      info->LastEntriesIdx = (info->LastEntriesIdx + 1) %
  IM_ARRAYSIZE(info->LastEntriesBuf); entry =
  &info->LastEntriesBuf[info->LastEntriesIdx]; entry->FrameCount = frame_count;
      entry->AllocCount = entry->FreeCount = 0;
  }
  if (size != (size_t)-1)
  {
      entry->AllocCount++;
      info->TotalAllocCount++;
      //printf("[%05d] MemAlloc(%d) -> 0x%p\n", frame_count, size, ptr);
  }
  else
  {
      entry->FreeCount++;
      info->TotalFreeCount++;
      //printf("[%05d] MemFree(0x%p)\n", frame_count, ptr);
  }*/
}

//-----------------------------------------------------------------------------
// [SECTION] ImGuiTextBuffer, ImGuiTextIndex
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to
// 2013 doesn't have it.
#ifndef va_copy
#if defined(__GNUC__) || defined(__clang__)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#else
#define va_copy(dest, src) (dest = src)
#endif
#endif

char hString::EmptyString[1] = { 0 };

/**
 * @brief Append a string to the hString buffer.
 *
 * This function appends the provided string to the hString buffer.
 * It calculates the length of the string based on the provided end pointer (str_end) or using strlen if str_end is null.
 * If the buffer capacity is not enough to hold the new string, it resizes the buffer to accommodate the new string.
 * The string is then copied into the buffer, and a zero-terminator is added.
 *
 * @param str Pointer to the beginning of the string to append.
 * @param str_end Pointer to the end of the string to append (optional).
 */
void hString::append(const char *str, const char *str_end) {
  // Calculate the length of the string
  int len = str_end ? (int)(str_end - str) : (int)strlen(str);

  // Calculate the write offset and needed size
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;

  // Check if the buffer capacity is enough, resize if needed
  if (needed_sz >= Buf.Capacity) {
    // Double the capacity or use the needed size, whichever is greater
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  // Resize the buffer to accommodate the new string
  Buf.resize(needed_sz);

  // Copy the string into the buffer
  memcpy(&Buf[write_off - 1], str, (size_t)len);

  // Add zero-terminator
  Buf[write_off - 1 + len] = 0;
}

/**
 * @brief Check the version string and data layout for debugging purposes.
 *
 * This function compares the provided version string with the defined VORTEX_VERSION.
 * If the strings do not match, it sets an error flag and asserts.
 *
 * @param version The version string to check.
 * @return True if the version string matches, false otherwise.
 */
bool VortexMaker::DebugCheckVersionAndDataLayout(const char *version) {
  bool error = false;

  // Check if the provided version string matches the defined VORTEX_VERSION
  if (strcmp(version, VORTEXLAUNCHER_VERSION) != 0) {
    error = true;
    // Assertion for debugging purposes, will only trigger in debug builds
    VX_ASSERT(strcmp(version, VORTEXLAUNCHER_VERSION) == 0 && "Mismatched version string!");
  }

  return !error;  // Return true if no error occurred
}

VortexNet::VortexNet() {
  naettInit(nullptr);
}

VortexNet::~VortexNet() {
}

bool VortexNet::CheckNet() {
  std::string res = GET("http://api.infinite.si:8000/");
  return !res.empty();
}

std::string VortexNet::GET(const std::string &url) {
  return Request(url, "GET");
}

std::string VortexNet::POST(const std::string &url, const std::string &body, const std::string &contentType) {
  return Request(url, "POST", body, contentType);
}





std::string VortexNet::Request(
    const std::string &url,
    const std::string &method,
    const std::string &body,
    const std::string &contentType) {

    naettReq* req = nullptr;

    if (method == "GET") {
        std::cout << "GET request\n";

    const char* URL = url.c_str();
req = naettRequest_va(
    URL,
    2,
    naettMethod("GET"),
    naettHeader("accept", "*/*")
);


    } else if (method == "POST") {
        std::cout << "POST request\n";


    } else {
        std::cerr << "Unsupported HTTP method: " << method << std::endl;
        return "";
    }

    if (!req) {
        std::cerr << "Failed to create request\n";
        return "";
    }

    naettRes* res = naettMake(req);
    if (!res) {
        std::cerr << "Failed to make request\n";
        naettFree(req);
        return "";
    }

    while (!naettComplete(res)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    int status = naettGetStatus(res);
    if (status < 0) {
        std::cerr << "Request failed with status: " << status << std::endl;
        naettClose(res);
        naettFree(req);
        return "";
    }

    int length = 0;
    const char* responseBody = static_cast<const char*>(naettGetBody(res, &length));
    if (!responseBody || length == 0) {
        std::cerr << "Empty response body\n";
        naettClose(res);
        naettFree(req);
        return "";
    }

    std::string result(responseBody, length);

    naettClose(res);
    naettFree(req);

    return result;
}

#endif  // VORTEX_DISABLE