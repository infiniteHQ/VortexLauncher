//
//  project_sessions.hpp
//  Headers for project sessions metrics and managment
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#ifndef VORTEX_PROJECT_SESSION_HPP
#define VORTEX_PROJECT_SESSION_HPP

namespace sessions {

  namespace fs = std::filesystem;
  using json = nlohmann::json;

  inline fs::path ListPath() {
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    return fs::path(home ? home : ".") / ".vx" / "projects" / "list.json";
  }

  inline std::string Key(const fs::path& p) {
    std::error_code ec;
    std::string s = fs::weakly_canonical(p, ec).generic_string();
    while (s.size() > 1 && s.back() == '/')
      s.pop_back();
#ifdef _WIN32
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
#endif
    return s;
  }

  inline std::time_t ParseIso(const std::string& s) {
    std::tm tm{};
    if (std::sscanf(
            s.c_str(), "%d-%d-%dT%d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
      return 0;
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
#ifdef _WIN32
    return _mkgmtime(&tm);
#else
    return timegm(&tm);
#endif
  }

  inline json Load() {
    std::ifstream in(ListPath());
    if (!in)
      return json::array();
    try {
      json j = json::parse(in);
      return j.is_array() ? j : json::array();
    } catch (...) {
      return json::array();
    }
  }

  inline void Save(const json& j) {
    fs::path file = ListPath();
    std::error_code ec;
    fs::create_directories(file.parent_path(), ec);
    fs::path tmp = file;
    tmp += ".tmp";
    {
      std::ofstream out(tmp, std::ios::trunc);
      out << j.dump(2);
    }
    fs::rename(tmp, file, ec);
  }

  inline json* Find(json& list, const std::string& key) {
    for (auto& e : list)
      if (e.value("project", "") == key)
        return &e;
    return nullptr;
  }

  inline bool IsOpen(const json& entry) {
    std::time_t ping = ParseIso(entry.value("last_ping", ""));
    return ping != 0 && std::difftime(std::time(nullptr), ping) < 5 * 60;
  }

  inline bool IsProjectOpen(const std::string& path) {
    json list = Load();
    json* e = Find(list, Key(path));
    return e && IsOpen(*e);
  }

  template<class Projects, class PathOf>
  void SyncAndSort(Projects& projects, PathOf path_of) {
    json list = Load();
    bool changed = false;

    for (auto& p : projects) {
      std::string key = Key(path_of(p));
      if (!Find(list, key)) {
        list.push_back({ { "project", key }, { "last_opened", "" }, { "last_ping", "" } });
        changed = true;
      }
    }
    if (changed)
      Save(list);

    auto last_opened = [&](auto& p) {
      json* e = Find(list, Key(path_of(p)));
      return e ? e->value("last_opened", "") : std::string();
    };
    std::stable_sort(projects.begin(), projects.end(), [&](auto& a, auto& b) { return last_opened(a) > last_opened(b); });
  }

}  // namespace sessions

#endif  // VORTEX_PROJECT_SESSION_HPP