//
//  launcher_version_refresh.hpp
//  Headers for launcher version change detection
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <string>

#include "vortex.h"

#ifndef VORTEX_LAUNCHER_VERSION_REFRESH_HPP
#define VORTEX_LAUNCHER_VERSION_REFRESH_HPP

namespace VortexMaker {
  VORTEX_API std::string GetLauncherVersionFilePath();
  VORTEX_API std::string ReadLauncherDataVersion();
  VORTEX_API bool WriteLauncherDataVersion(const std::string& version);
  VORTEX_API bool CheckLauncherVersionAndRefresh();
  VORTEX_API void RefreshEnvironmentForLauncher();
}  // namespace VortexMaker

#endif  // VORTEX_LAUNCHER_VERSION_REFRESH_HPP