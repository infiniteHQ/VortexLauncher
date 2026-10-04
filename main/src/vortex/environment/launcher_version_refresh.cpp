//
//  launcher_version_refresh.hpp
//  Sources for launcher version change detection
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <filesystem>
#include <string>

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

namespace VortexMaker {

  std::string GetLauncherVersionFilePath() {
    std::filesystem::path file = std::filesystem::path(getHomeDirectory()) / ".vx" / "data" / "data_updated_to_version.json";
    return file.string();
  }

  std::string ReadLauncherDataVersion() {
    const std::string file = GetLauncherVersionFilePath();

    if (!std::filesystem::exists(file))
      return "";

    try {
      nlohmann::json data = DumpJSON(file);
      if (data.is_object() && data.contains("version") && data["version"].is_string())
        return data["version"].get<std::string>();
    } catch (...) {
    }

    return "";
  }

  bool WriteLauncherDataVersion(const std::string& version) {
    const std::string file = GetLauncherVersionFilePath();

    try {
      createFolderIfNotExists(std::filesystem::path(file).parent_path().string());
      PopulateJSON(nlohmann::json{ { "version", version } }, file);
      return true;
    } catch (...) {
      return false;
    }
  }

  bool CheckLauncherVersionAndRefresh() {
    const std::string current = VORTEXLAUNCHER_VERSION;
    const std::string stored = ReadLauncherDataVersion();

    if (stored == current)
      return false;

    if (stored.empty())
      LogInfo("Launcher", "No valid version data found, refreshing environment for version " + current);
    else
      LogInfo("Launcher", "Launcher version changed (" + stored + " -> " + current + "), refreshing environment");

    RefreshEnvironmentForLauncher();

    if (!WriteLauncherDataVersion(current))
      LogWarn("Launcher", "Unable to update " + GetLauncherVersionFilePath());

    return true;
  }

}  // namespace VortexMaker