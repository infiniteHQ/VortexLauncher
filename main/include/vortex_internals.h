// The VortexMaker Project
// [internal structures]

// See licence in LICSENCE.md
// (c) 2022-2024, Diego E. Moreno

//_____________________________________________________________________________
// Informations about this file
//_____________________________________________________________________________
// This file have all primitives types of vortex runtime & api.
// There is the low level part of the project.
// Please read introductions & informations on vortex.h before do anything.
//_____________________________________________________________________________

#pragma once
#ifndef VORTEX_DISABLE

//_____________________________________________________________________________
// [SECTION] Header mess
//_____________________________________________________________________________
// Includes

#ifndef VORTEX_VERSION
#include "vortex.h"
#endif

#include "./modules/interface.h"
#include "./templates/install.h"
#include "./templates/interface.h"

//_____________________________________________________________________________

// Enable SSE intrinsics if available
#if (defined __SSE__ || defined __x86_64__ || defined _M_X64 || (defined(_M_IX86_FP) && (_M_IX86_FP >= 1))) && !defined(IMGUI_DISABLE_SSE)
#define IMGUI_ENABLE_SSE
#include <immintrin.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)           // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when IMGUI_API is set to__declspec(dllexport)
#pragma warning(disable : 26812)          // The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3). [MSVC Static Analyzer)
#pragma warning(disable : 26495)          // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning(disable : 5054)           // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option" // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"    // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wfloat-equal"        // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok, for ImFloor()
#pragma clang diagnostic ignored "-Wunused-function"    // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes" // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion" // warning: implicit conversion from 'xxx' to 'float' may lose precision
#pragma clang diagnostic ignored "-Wmissing-noreturn"              // warning: function 'xxx' could be declared with attribute 'noreturn'
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"         // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess" // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//_____________________________________________________________________________
// [SECTION] Forward declarations
//_____________________________________________________________________________
// Absolute basis
struct VxContext;

//_____________________________________________________________________________
// [SECTION] Main runtime pointer
// See implementation of this variable in imgui.cpp for comments and details.
//_____________________________________________________________________________
#ifndef CVortexMaker
extern VORTEX_API VxContext *CVortexMaker; // Current implicit context pointer
#endif
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION]: Macros
//_____________________________________________________________________________
// Static Asserts
#define Vx_STATIC_ASSERT(_COND) static_assert(_COND, "")
//_____________________________________________________________________________

//_____________________________________________________________________________
// [SECTION]: Generic helpers & utils
//_____________________________________________________________________________

#define Vx_STRING_TO_INT(_str, _h) str2int(_str, _h);
#define Vx_BASE64ENCODE(_toencodebuffer) base64_encode(_toencodebuffer);
#define Vx_BASE64DECODE(_todecodebuffer) base64_decode(_todecodebuffer);

//_____________________________________________________________________________

struct VortexMakerDebugAllocInfo
{
  int TotalAllocCount; // Number of call to MemAlloc().
  int TotalFreeCount;
  VortexMakerDebugAllocInfo() { memset(this, 0, sizeof(*this)); }
};

//_____________________________________________________________________________
// [SECTION]: Internal structures
//_____________________________________________________________________________

struct EnvProject
{
  std::string name;
  std::string path;
  std::string version;
  std::string compatibleWith;
  std::string description;
  std::string logoPath;
  std::string author;
  std::string lastOpened;
};


struct VortexNews
{
  std::string topic;
  std::string title;
  std::string description;
  std::string image_link;
  std::string news_link; // to redirect
};

struct VxSystemLog
{
  spdlog::level::level_enum m_level;
  std::string m_filter;
  std::string m_message;
  std::string m_timestamp;

  VxSystemLog(spdlog::level::level_enum level, std::string filter, std::string message) : m_level(level), m_filter(filter), m_message(message){};
};

struct SessionState
{
  //
  std::string session_id;

  // Master
  std::string session_started_at;
  std::string session_started_on_os;
  std::string session_started_on_arch;

  // Last module
  std::shared_ptr<ModuleInterface> last_used_module;

  bool master_initialized = false;
  bool logs_modified = false;
  bool last_used_module_modified = false;
  bool last_used_plugin_modified = false;
  bool last_used_module_input_event_modified = false;
  bool last_used_module_output_event_modified = false;
};

struct VxIO
{
  int MetricsActiveAllocations;

  // EM / Editor Modules
  std::vector<void *> em_handles;
  std::vector<std::shared_ptr<ModuleInterface>> em;
  std::vector<std::shared_ptr<ModuleInterface>> sys_em;

  // Know projects in system
  std::vector<std::shared_ptr<EnvProject>> sys_projects;

  // Pools
  std::vector<std::string> sys_projects_pools;
  std::vector<std::string> sys_vortex_versions_pools;
  std::vector<std::string> sys_modules_pools;
  std::vector<std::string> sys_templates_pools;
  std::vector<std::string> sys_plugins_pools;
  std::vector<std::shared_ptr<VortexVersion>> sys_vortex_version;
  std::vector<std::string> sys_available_versions;

  // Dists
  std::vector<std::string> sys_vortex_dists;
  std::string sys_vortexlauncher_dist;

  // Templates
  std::vector<std::shared_ptr<TemplateInterface>> templates;
  std::vector<std::shared_ptr<TemplateInterface>> sys_templates;

  // Misc
  std::vector<VortexNews> news;
  bool offline = false;
};

struct VxPaths
{
  std::string toolchainDistFolder;
  std::string hostDistFolder;
};

enum class PlatformVendor
{ 
    Linux,
    Windows,
    Macos
};

struct VortexLauncherVersion
{
  std::string version;
  std::string dist;
  std::string arch;
  std::string path;
  std::string sum;
  std::string created_at;
};

struct VortexVersion
{
  std::string version;
  std::string name;
  std::string path;
  std::string sum;
  std::string date;
  std::string banner;
  bool already_installed = false;
  bool working = false;
};

//-----------------------------------------------------------------------------
// (Context) VortexMakerContext => Main VortexMaker context.
//-----------------------------------------------------------------------------
// This context contain all user data about VortexMaker functionnal interfaces &
// all instances of custom contents.
//-----------------------------------------------------------------------------
struct VxContext
{// Master flags
  bool initialized;
  int session_count = 0;

  // Loger
  bool logger;
  bool logger_registering = true;
  std::shared_ptr<spdlog::logger> global_logger;
  std::shared_ptr<spdlog::logger> console_logger;
  std::vector<std::pair<std::string, std::shared_ptr<spdlog::logger>>> pool_loggers;

  // Vendor
  PlatformVendor m_PlatformVendor;
  // Paths 
  std::string m_VortexPath; // relative to ".vx"
  std::string m_VortexLauncherPath; // relative to the emplacement where the Vortex Launcher is.

  std::vector<VortexVersion> latest_vortex_versions;
  VortexLauncherVersion latest_launcher_version;
  VortexVersion latest_vortex_version;
  bool launcher_update_available = false;
  bool vortex_update_available = false;
  bool blankproject_exist = false;
  bool disconnected = false;

  // Components
  VxIO IO;
  SessionState state;
  VortexMakerDebugAllocInfo debugAllocInfo;
  std::vector<std::shared_ptr<VxSystemLog>> registered_logs;
  fs::path projectPath;
  fs::path logoPath;
  VxPaths paths;
  std::string configFilePath;
  std::string author;
  std::string compatibleWith;
  std::string description;
  std::string label;
  std::string name;
  std::string type;
  std::string version;
  std::string project_version;
  std::string toolchainsPath;
  std::string gposPath;
  std::string packagesPath;
  std::string scriptsPath;
  std::string hostsPath;

  std::string platform;
  std::string arch;
  bool include_system_templates;
};
//-----------------------------------------------------------------------------

//_____________________________________________________________________________

//__________________________________________________________________________________________________________________
// [SECTION]: Internal API of VortexMaker
//__________________________________________________________________________________________________________________
// Internals functions of HypernetTK. Contain all basic stuff and low level
// features. All of theses functions are called by upper level api & user-api.
// Basicly, you don't need to care about this. For all user interactions of
// Hypernet & Vx, go on the main userapi on vortex.h
//__________________________________________________________________________________________________________________
namespace VortexMaker
{

  // Utils & Base
  VORTEX_API void DebugAllocHook(VortexMakerDebugAllocInfo *info, void *ptr, size_t size); // size >= 0 : alloc, size = -1 : free
  
}
//_____________________________________________________________________________

#endif // VORTEX_DISABLE