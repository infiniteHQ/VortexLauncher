#include "./welcome.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>  // std::system
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_set>
static std::vector<bool> selectedRows(finded_projects.size(), false);

std::vector<std::string> modules_projects;
static std::vector<int> selectedIDs;

int projetct_import_dest_index;
std::vector<std::string> project_pools;
std::string projetct_import_dest;

static bool no_installed_modal_opened;
static bool multiple_versions_modal_opened;
static bool already_running_modal_opened;
static bool open_deletion_modal = false;
static bool project_deleted = false;

static std::string no_installed_version;
static std::string no_installed_project_name;
static std::string no_installed_project_picture;
static VortexVersion no_installed_version_available;
static std::vector<std::shared_ptr<VortexVersion>> all_versions_for_project;
#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

bool EndsWith(const std::string& value, const std::string& suffix) {
  if (suffix.size() > value.size())
    return false;
  return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

static std::string test;

// helpers
namespace {

  struct UiPalette  {
    bool dark;
    const char* card;
    const char* cardHover;
    const char* border;
    const char* text;
    const char* sub;
    const char* sep;
    const char* pillBg;
    const char* pillText;
    const char* accent;
    const char* accentText;
    const char* danger;
    const char* ok;
    const char* panel;
  };

  UiPalette  GetCreatePalette() {
    if (CherryApp.GetTheme() == "dark_vortex") {
      return { true,      "#232323", "#2C2C2C", "#333333", "#FFFFFF", "#8A8A8A", "#2A2A2A",
               "#303030", "#BBBBBB", "#B1FF31", "#121212", "#EE5555", "#B1FF31", "#35353535" };
    }
    return { false,     "#FFFFFF", "#F1F1F1", "#D8D8D8", "#232323", "#6B6B6B", "#DADADA",
             "#E6E6E6", "#444444", "#3E8E00", "#FFFFFF", "#D32F2F", "#3E8E00", "#DFDFDF" };
  }

  std::string LowerStr(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
  }

  std::string TrimStr(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos)
      return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
  }

  int TemplateKind(const std::string& type) {
    if (type == "project")
      return 0;
    if (type == "tool")
      return 1;
    return 2;
  }

  void CreatePill(const std::string& t, const char* bg, const char* fg) {
    ImVec2 ts = CherryGUI::CalcTextSize(t.c_str());
    ImVec2 size(ts.x + 14.0f, ts.y + 4.0f);
    ImVec2 p = CherryGUI::GetCursorScreenPos();
    ImDrawList* dl = CherryGUI::GetWindowDrawList();
    dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), Cherry::HexToImU32(bg), size.y * 0.5f);
    dl->AddText(ImVec2(p.x + 7.0f, p.y + 2.0f), Cherry::HexToImU32(fg), t.c_str());
    CherryGUI::Dummy(size);
  }

  enum class NameStatus { Empty, InvalidChars, Taken, FolderExists, Ok };

  bool IsAutoName(const std::string& s, const std::string& last_suggested) {
    if (s.empty() || s == last_suggested)
      return true;
    const std::string base = "New Project";
    if (s == base)
      return true;
    if (s.rfind(base + " ", 0) != 0)
      return false;
    const std::string rest = s.substr(base.size() + 1);
    return !rest.empty() && std::all_of(rest.begin(), rest.end(), [](unsigned char c) { return std::isdigit(c) != 0; });
  }

  std::vector<int> VersionParts(const std::string& v) {
    std::vector<int> parts;
    std::string cur;
    for (char c : v) {
      if (std::isdigit((unsigned char)c)) {
        cur += c;
      } else if (!cur.empty()) {
        parts.push_back(std::atoi(cur.c_str()));
        cur.clear();
      }
    }
    if (!cur.empty())
      parts.push_back(std::atoi(cur.c_str()));
    return parts;
  }

  bool VersionLess(const std::string& a, const std::string& b) {
    return VersionParts(a) < VersionParts(b);
  }

  std::string LatestVersion(const std::vector<std::string>& versions) {
    if (versions.empty())
      return "";
    return *std::max_element(versions.begin(), versions.end(), VersionLess);
  }

  bool HasVersion(const std::vector<std::string>& versions, const std::string& v) {
    return std::find(versions.begin(), versions.end(), v) != versions.end();
  }

  int ProjectVersionState(const std::string& version) {
    static std::unordered_map<std::string, int> cache;
    static double last_refresh = -100.0;
    const double now = CherryGUI::GetTime();
    if (now - last_refresh > 2.0) {
      cache.clear();
      last_refresh = now;
    }
    auto it = cache.find(version);
    if (it != cache.end())
      return it->second;

    std::string versionpath;
    int state;
    if (VortexMaker::CheckIfVortexVersionUtilityExist(version, versionpath)) {
      state = 0;
    } else if (VortexMaker::CheckVersionAvailibility(version).version == "") {
      state = 2;
    } else {
      state = 1;
    }
    cache[version] = state;
    return state;
  }

  const char* ProjectStateColor(int state, const UiPalette& pal) {
    if (state == 0)
      return pal.sub;
    if (state == 1)
      return pal.dark ? "#EEAA55" : "#C77700";
    return pal.danger;
  }

  std::string ProjectLogoPath(const std::string& path) {
#ifdef _WIN32
    return VortexMaker::convertPathToWindowsStyle(path);
#else
    return path;
#endif
  }

  const std::unordered_map<std::string, std::string>& ProjectLastOpenedMap(
      const std::vector<std::shared_ptr<EnvProject>>& projects) {
    static std::unordered_map<std::string, std::string> cache;
    static double last_refresh = -100.0;
    const double now = CherryGUI::GetTime();
    if (now - last_refresh > 1.0) {
      last_refresh = now;
      cache.clear();

      std::unordered_map<std::string, std::string> by_key;
      for (auto& e : sessions::Load()) {
        by_key[e.value("project", "")] = e.value("last_opened", "");
      }
      for (auto& p : projects) {
        if (!p)
          continue;
        auto it = by_key.find(sessions::Key(p->path));
        cache[p->path] = (it != by_key.end()) ? it->second : std::string();
      }
    }
    return cache;
  }

  std::vector<std::shared_ptr<EnvProject>> RecentProjectsBySession(size_t max_count) {
    const auto& all = VortexMaker::GetCurrentContext()->IO.sys_projects;
    const auto& last_opened = ProjectLastOpenedMap(all);

    std::vector<std::shared_ptr<EnvProject>> out;
    for (auto& p : all) {
      if (!p)
        continue;
      auto it = last_opened.find(p->path);
      if (it != last_opened.end() && !it->second.empty())
        out.push_back(p);
    }

    std::stable_sort(
        out.begin(), out.end(), [&](const std::shared_ptr<EnvProject>& a, const std::shared_ptr<EnvProject>& b) {
          return last_opened.at(a->path) > last_opened.at(b->path);
        });

    if (out.size() > max_count)
      out.resize(max_count);
    return out;
  }

  std::string FormatLastOpened(const std::string& path) {
    const auto& all = VortexMaker::GetCurrentContext()->IO.sys_projects;
    const auto& last_opened = ProjectLastOpenedMap(all);
    auto it = last_opened.find(path);
    if (it == last_opened.end() || it->second.empty())
      return "";
    std::string s = it->second;
    std::replace(s.begin(), s.end(), 'T', ' ');
    return s.substr(0, 16);
  }

}  // namespace

namespace VortexLauncher {

  void WelcomeWindow::CreateProjectRender() {
    const UiPalette pal = GetCreatePalette();

    static char search[128] = "";
    static int filter = -1;
    static bool advanced_params = false;
    static bool needs_suggest = true;
    static std::string last_suggested;
    const float right_w = 360.0f;
    const float total_x = CherryGUI::GetContentRegionAvail().x;

    CherryStyle::AddMarginX(5.0f);
    CherryGUI::BeginChild("###create_left", ImVec2(total_x - right_w - 5.0f, 0), false, ImGuiWindowFlags_NoBackground);

    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", pal.sub);
    CherryKit::TitleFive(Cherry::GetLocale("loc.menubar.welcome.select_a_base_and_create"));
    Cherry::PopFont();
    CherryNextProp("color", pal.sep);
    CherryKit::Separator();

    int counts[3] = { 0, 0, 0 };
    for (auto& tpl : project_templates) {
      if (tpl)
        counts[TemplateKind(tpl->m_type)]++;
    }

    CherryStyle::AddMarginY(4.0f);
    CherryGUI::SetNextItemWidth(260.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
    CherryGUI::PushStyleColor(ImGuiCol_FrameBg, Cherry::HexToRGBA(pal.card));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA(pal.border));
    CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.text));
    CherryGUI::InputTextWithHint("##create_search", "Search a template...", search, sizeof(search));
    CherryGUI::PopStyleColor(3);
    CherryGUI::PopStyleVar(2);

    auto chip = [&](const std::string& label, int value) {
      const bool active = (filter == value);
      CherryGUI::SameLine(0.0f, 6.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.0f);
      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA(active ? pal.accent : pal.pillBg));
      CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(active ? pal.accentText : pal.pillText));
      if (CherryGUI::Button(label.c_str()))
        filter = value;
      CherryGUI::PopStyleColor(2);
      CherryGUI::PopStyleVar();
    };
    chip("All (" + std::to_string(counts[0] + counts[1] + counts[2]) + ")", -1);
    chip(Cherry::GetLocale("loc.project") + " (" + std::to_string(counts[0]) + ")", 0);
    chip(Cherry::GetLocale("loc.tool") + " (" + std::to_string(counts[1]) + ")", 1);
    if (counts[2] > 0)
      chip("Other (" + std::to_string(counts[2]) + ")", 2);

    CherryGUI::NewLine();

    CherryGUI::BeginChild("###create_list", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground);

    const std::string query = LowerStr(search);
    const std::string section_titles[3] = { Cherry::GetLocale("loc.project_template"),
                                            Cherry::GetLocale("loc.tool_template"),
                                            Cherry::GetLocale("loc.template") };

    bool anything_shown = false;
    int uid = 0;

    for (int kind = 0; kind < 3; kind++) {
      if (filter != -1 && filter != kind)
        continue;

      std::vector<decltype(project_templates.begin())> items;
      for (auto it = project_templates.begin(); it != project_templates.end(); ++it) {
        auto& tpl = *it;
        if (!tpl || TemplateKind(tpl->m_type) != kind)
          continue;
        if (!query.empty()) {
          if (LowerStr(tpl->m_proper_name).find(query) == std::string::npos &&
              LowerStr(tpl->m_name).find(query) == std::string::npos &&
              LowerStr(tpl->m_description).find(query) == std::string::npos) {
            continue;
          }
        }
        items.push_back(it);
      }
      if (items.empty())
        continue;
      anything_shown = true;

      CherryStyle::AddMarginY(6.0f);
      Cherry::PushFont("ClashBold");
      CherryNextProp("color_text", pal.text);
      CherryKit::TitleSix(section_titles[kind]);
      Cherry::PopFont();
      CherryGUI::SameLine();
      CherryNextProp("color_text", pal.sub);
      CherryKit::TextSimple("(" + std::to_string(items.size()) + ")");
      CherryNextProp("color", pal.sep);
      CherryKit::Separator();
      CherryStyle::AddMarginY(4.0f);

      for (auto it : items) {
        auto& tpl = *it;
        const bool selected = selected_template_object && selected_template_object->m_name == tpl->m_name;
        const float card_h = 62.0f;
        const float card_w = CherryGUI::GetContentRegionAvail().x - 6.0f;

        ImVec2 p = CherryGUI::GetCursorScreenPos();
        const bool hovered = CherryGUI::IsMouseHoveringRect(p, ImVec2(p.x + card_w, p.y + card_h));
        bool card_clicked = false;

        CherryGUI::PushID(uid++);
        CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA(hovered ? pal.cardHover : pal.card));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA(selected ? pal.accent : pal.border));
        CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, selected ? 2.0f : 1.0f);

        if (CherryGUI::BeginChild(
                "##tpl_card",
                ImVec2(card_w, card_h),
                true,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
          CherryGUI::SetCursorPos(ImVec2(12.0f, (card_h - 40.0f) * 0.5f));
          CherryKit::ImageLocal(tpl->m_logo_path, 40.0f, 40.0f);
          CherryGUI::SameLine(0.0f, 12.0f);

          CherryGUI::BeginGroup();
          CherryGUI::SetCursorPosY(10.0f);
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", pal.text);
          CherryKit::TitleSix(tpl->m_proper_name);
          Cherry::PopFont();
          if (!tpl->m_compatible_versions.empty()) {
            const size_t extra = tpl->m_compatible_versions.size() - 1;
            CherryGUI::SameLine(0.0f, 8.0f);
            CreatePill(LatestVersion(tpl->m_compatible_versions), pal.pillBg, pal.pillText);
            if (extra > 0) {
              CherryGUI::SameLine(0.0f, 6.0f);
              CherryNextProp("color_text", pal.sub);
              CherryStyle::AddMarginY(2.0f);
              CherryKit::TextSimple("+" + std::to_string(extra));
            }
          }
          CherryNextProp("color_text", pal.sub);
          CherryKit::TextSimple(tpl->m_description);
          CherryGUI::EndGroup();

          if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseClicked(0)) {
            card_clicked = true;
          }
        }
        CherryGUI::EndChild();

        CherryGUI::PopStyleVar(2);
        CherryGUI::PopStyleColor(2);
        CherryGUI::PopID();

        if (card_clicked) {
          selected_template_object = tpl;
          std::sort(
              selected_template_object->m_compatible_versions.begin(),
              selected_template_object->m_compatible_versions.end(),
              [](const std::string& a, const std::string& b) { return VersionLess(b, a); });
          if (IsAutoName(v_ProjectName, last_suggested)) {
            v_ProjectName.clear();
            needs_suggest = true;
          }
        }
        CherryStyle::AddMarginY(4.0f);
      }
    }

    if (!anything_shown) {
      CherryKit::Space(20.0f);
      CherryNextProp("color_text", pal.sub);
      CherryKit::TextCenter("No template matches your search.");
    }

    CherryGUI::EndChild();  // ###create_list
    CherryGUI::EndChild();  // ###create_left

    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA(pal.panel));
    CherryGUI::BeginChild("right", ImVec2(right_w, 0));

    if (!selected_template_object) {
      auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
      auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      CherryGUI::SetCursorPos(ImVec2((child_size.x - texture_size.x) / 2.0f, (child_size.y - texture_size.y) / 2.0f));
      CherryGUI::Image(texture, texture_size);
    } else {
      CherryGUI::Indent(14.0f);
      const float inner_w = CherryGUI::GetContentRegionAvail().x - 14.0f;

      std::string effective_version = LatestVersion(selected_template_object->m_compatible_versions);
      {
        std::string sel = Cherry::GetData(CherryID("VersionSelector"), "selected_string");
        if (sel != "undefined" && HasVersion(selected_template_object->m_compatible_versions, sel)) {
          effective_version = sel;
        }
      }

      CherryStyle::AddMarginY(14.0f);
      CherryKit::ImageLocal(selected_template_object->m_logo_path, 50.0f, 50.0f);
      CherryGUI::SameLine(0.0f, 12.0f);
      CherryGUI::BeginGroup();
      Cherry::PushFont("ClashBold");
      CherryNextProp("color_text", pal.text);
      CherryKit::TitleFive(selected_template_object->m_proper_name);
      Cherry::PopFont();

      std::string project_type;
      if (selected_template_object->m_type == "project") {
        project_type = Cherry::GetLocale("loc.project_template");
      } else if (selected_template_object->m_type == "tool") {
        project_type = Cherry::GetLocale("loc.tool_template");
      } else {
        project_type = Cherry::GetLocale("loc.template");
      }
      CreatePill(project_type, pal.accent, pal.accentText);
      if (!effective_version.empty()) {
        CherryGUI::SameLine(0.0f, 6.0f);
        CreatePill("Vortex " + effective_version, pal.pillBg, pal.pillText);
      }
      CherryGUI::EndGroup();

      CherryStyle::AddMarginY(8.0f);
      CherryNextProp("color", pal.sep);
      CherryKit::Separator();

      CherryNextProp("color_text", pal.sub);
      CherryKit::TextWrapped(selected_template_object->m_description);

      CherryStyle::AddMarginY(8.0f);
      CherryNextProp("color_text", pal.accent);
      if (CherryKit::ButtonText(advanced_params ? "Hide advanced settings" : "Advanced settings").GetData("isClicked") ==
          "true") {
        advanced_params = !advanced_params;
      }

      if (advanced_params) {
        Cherry::SetNextComponentProperty("header_visible", "false");
        CherryKit::TableSimple(
            "Project props inputs",
            {
                CherryKit::KeyValString(Cherry::GetLocale("loc.description"), &v_ProjectDescription),
                CherryKit::KeyValString(Cherry::GetLocale("loc.author"), &v_ProjectAuthor),
                CherryKit::KeyValString(Cherry::GetLocale("loc.version"), &v_ProjectVersion),
                CherryKit::KeyValComboString(
                    CherryID("VersionSelector"),
                    Cherry::GetLocale("loc.vortex_version"),
                    &selected_template_object->m_compatible_versions),
                CherryKit::KeyValComboString(
                    CherryID("PathSelector"),
                    Cherry::GetLocale("loc.project_path"),
                    &projectPoolsPaths),  // TODO : Take this by the index, not the cherry datas
            });
      }

      std::string dest_base;
      {
        std::string sel = Cherry::GetData(CherryID("PathSelector"), "selected_string");
        if (sel != "undefined") {
          dest_base = sel;
        } else if (!projectPoolsPaths.empty()) {
          dest_base = projectPoolsPaths.back();
        }
      }

      const bool do_suggest = needs_suggest && IsAutoName(v_ProjectName, last_suggested);
      needs_suggest = false;
      if (do_suggest) {
        auto is_free = [&](const std::string& candidate) {
          const std::string l = LowerStr(candidate);
          for (auto& e : VortexMaker::GetCurrentContext()->IO.sys_projects) {
            if (e && LowerStr(TrimStr(e->name)) == l)
              return false;
          }
          if (!dest_base.empty()) {
            std::error_code ec;
            if (fs::exists(fs::path(dest_base) / candidate, ec))
              return false;
          }
          return true;
        };

        std::string candidate = "New Project";
        for (int n = 2; !is_free(candidate) && n < 10000; n++) {
          candidate = "New Project " + std::to_string(n);
        }
        v_ProjectName = candidate;
        last_suggested = candidate;
      }

      const std::string name = TrimStr(v_ProjectName);

      NameStatus status = NameStatus::Ok;
      if (name.empty()) {
        status = NameStatus::Empty;
      } else if (name.find_first_of("\\/:*?\"<>|") != std::string::npos) {
        status = NameStatus::InvalidChars;
      } else {
        const std::string lname = LowerStr(name);
        for (auto& existing : VortexMaker::GetCurrentContext()->IO.sys_projects) {
          if (existing && LowerStr(TrimStr(existing->name)) == lname) {
            status = NameStatus::Taken;
            break;
          }
        }
        if (status == NameStatus::Ok && !dest_base.empty()) {
          std::error_code ec;
          if (fs::exists(fs::path(dest_base) / name, ec))
            status = NameStatus::FolderExists;
        }
      }

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      const float footer_h = 110.0f;
      CherryGUI::SetCursorPosY(windowSize.y - footer_h);

      CherryNextProp("color", pal.sep);
      CherryKit::Separator();
      CherryStyle::AddMarginY(6.0f);

      CherryNextProp("color_text", pal.sub);
      CherryKit::TextSimple(Cherry::GetLocale("loc.name"));

      Cherry::SetNextComponentProperty("padding_y", "6.5");
      Cherry::SetNextComponentProperty("size_x", std::to_string(inner_w - 100.0f));
      CherryKit::InputString("####create_project_name", &v_ProjectName);
      CherryGUI::SameLine();

      const bool can_create = (status == NameStatus::Ok);
      if (!can_create)
        CherryGUI::BeginDisabled();

      if (CherryKit::ButtonImageText(Cherry::GetLocale("loc.create"), Cherry::GetPath("resources/base/add.png"))
                  .GetData("isClicked") == "true" &&
          can_create) {
        std::string vx_version;

        vx_version = effective_version.empty() ? "Undefined" : effective_version;
        if (Cherry::GetData(CherryID("PathSelector"), "selected_string") != "undefined") {
#ifdef _WIN32
          std::string creation_path = Cherry::GetData(CherryID("PathSelector"), "selected_string") + "\\" + v_ProjectName;
          creation_path = VortexMaker::convertPathToWindowsStyle(creation_path);
#else
          std::string creation_path = Cherry::GetData(CherryID("PathSelector"), "selected_string") + "/" + v_ProjectName;
#endif

#ifdef _WIN32
          std::string image_path = "\\icon.png";
#else
          std::string image_path = "/icon.png";
#endif
          // TODO error managment
          VortexMaker::CreateProject(
              v_ProjectName,
              v_ProjectAuthor,
              vx_version,
              v_ProjectVersion,
              v_ProjectDescription,
              creation_path,
              creation_path + image_path,
              selected_template_object->m_name);
          project_blocks.clear();
          VortexMaker::RefreshEnvironmentProjects();
          m_SelectedChildName = "?loc:loc.windows.welcome.open_project";
          m_SelectedEnvproject = nullptr;
          for (auto element : VortexMaker::GetCurrentContext()->IO.sys_projects) {
            if (fs::weakly_canonical(element->path) == fs::weakly_canonical(creation_path)) {
              m_SelectedEnvproject = element;
              break;
            }
          }

          v_ProjectName = "";
          v_ProjectAuthor = "";
          v_ProjectVersion = "";
          v_ProjectDescription = "";
        } else {
#ifdef _WIN32
          std::string creation_path = projectPoolsPaths.back() + "\\" + v_ProjectName + "\\";
          creation_path = VortexMaker::convertPathToWindowsStyle(creation_path);
#else
          std::string creation_path = projectPoolsPaths.back() + "/" + v_ProjectName + "/";
#endif

#ifdef _WIN32
          std::string image_path = "\\icon.png";
#else
          std::string image_path = "/icon.png";
#endif

          if (!projectPoolsPaths.empty()) {
            VortexMaker::CreateProject(
                v_ProjectName,
                v_ProjectAuthor,
                vx_version,
                v_ProjectVersion,
                v_ProjectDescription,
                creation_path,
                creation_path + image_path,
                selected_template_object->m_name);
            project_blocks.clear();
            VortexMaker::RefreshEnvironmentProjects();
            m_SelectedEnvproject = nullptr;
            m_SelectedChildName = "?loc:loc.windows.welcome.open_project";
            for (auto element : VortexMaker::GetCurrentContext()->IO.sys_projects) {
              if (fs::weakly_canonical(element->path) == fs::weakly_canonical(creation_path)) {
                m_SelectedEnvproject = element;
                break;
              }
            }

            v_ProjectName = "";
            v_ProjectAuthor = "";
            v_ProjectVersion = "";
            v_ProjectDescription = "";
          } else {
            std::cout << "Unable to create a project, no project pools are founded !" << std::endl;
          }
        }
        if (v_ProjectName.empty())
          needs_suggest = true;
      }

      if (!can_create)
        CherryGUI::EndDisabled();

      switch (status) {
        case NameStatus::Empty:
          CherryNextProp("color_text", pal.sub);
          CherryKit::TextSimple("Enter a name for your project.");
          break;
        case NameStatus::InvalidChars:
          CherryNextProp("color_text", pal.danger);
          CherryKit::TextSimple("Name contains invalid characters:  \\ / : * ? \" < > |");
          break;
        case NameStatus::Taken:
          CherryNextProp("color_text", pal.danger);
          CherryKit::TextSimple("A project named \"" + name + "\" already exists.");
          break;
        case NameStatus::FolderExists:
          CherryNextProp("color_text", pal.danger);
          CherryKit::TextSimple("A folder with this name already exists in the destination.");
          break;
        case NameStatus::Ok: break;
      }

      CherryGUI::Unindent(14.0f);
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void WelcomeWindow::OpenProjectRender() {
    const UiPalette pal = GetCreatePalette();

    static bool show_filters = false;
    static char search[128] = "";
    static int type_filter = 0;
    static std::string version_filter;
    static int sort_mode = 0;
    static bool list_view = false;
    static int refresh_frames = 0;

    auto do_open = [this](std::shared_ptr<EnvProject> project) {
      std::string versionpath;
      bool version_exist = VortexMaker::CheckIfVortexVersionUtilityExist(project->compatibleWith, versionpath);
      if (!version_exist) {
        no_installed_version = project->compatibleWith;
        no_installed_project_name = project->name;
        no_installed_project_picture = project->logoPath;

        no_installed_version_available = VortexMaker::CheckVersionAvailibility(project->compatibleWith);

        no_installed_modal_opened = true;
      } else {
        RequestOpen(project);
      }
    };

    auto do_delete = [this](std::shared_ptr<EnvProject> project) {
      m_SelectedEnvprojectToRemove = project;
      open_deletion_modal = true;
    };

    auto chip = [&](const std::string& label, bool active, float pad_x = 14.0f, float pad_y = 6.0f) -> bool {
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(pad_x, pad_y));
      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA(active ? pal.accent : pal.pillBg));
      CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(active ? pal.accentText : pal.pillText));
      const bool clicked = CherryGUI::Button(label.c_str());
      CherryGUI::PopStyleColor(2);
      CherryGUI::PopStyleVar(2);
      return clicked;
    };

    float bottom_pan = 130.0f;
    float avail_x = CherryGUI::GetContentRegionAvail().x;
    float avail_y = CherryGUI::GetContentRegionAvail().y - bottom_pan;

    CherryGUI::BeginChild(
        "left_pane", ImVec2(avail_x, avail_y), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", pal.sub);
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleFive(Cherry::GetLocale("loc.windows.welcome.latest_projects_tools"));
    Cherry::PopFont();

    CherryGUI::SameLine();
    CherryStyle::AddMarginX(10.0f);
    Cherry::SetNextComponentProperty("padding_x", "8");
    Cherry::SetNextComponentProperty("padding_y", "4");

    if (CherryKit::ButtonImageText(Cherry::GetLocale("loc.import"), Cherry::GetPath("resources/base/add.png"))
            .GetData("isClicked") == "true") {
      m_AssetFinder =
          AssetFinder::Create(Cherry::GetLocale("loc.menubar.title.import_projects"), VortexMaker::getHomeDirectory());
      Cherry::ApplicationSpecification spec;

      std::string name = "Import project(s)";
      spec.Name = name;
      spec.MinHeight = 500;
      spec.MinWidth = 500;
      spec.Height = 600;
      spec.Width = 1150;
      spec.DisableTitle = false;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = false;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
      spec.UniqueAppWindowName = m_AssetFinder->GetAppWindow()->m_Name;
      spec.UsingCloseCallback = true;
      spec.FavIconPath = Cherry::GetPath("resources/imgs/vproject.png");
      spec.IconPath = Cherry::GetPath("resources/imgs/vproject.png");
      spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
      spec.WindowSaves = false;
      spec.MenubarCallback = [this]() {
        if (CherryGUI::BeginMenu(Cherry::GetLocale("loc.window").c_str())) {
          if (CherryGUI::MenuItem(Cherry::GetLocale("loc.close").c_str())) {
            Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
          }
          CherryGUI::EndMenu();
        }
      };

      m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);
      m_AssetFinder->m_ElementName = "";

      if (!VortexMaker::GetCurrentContext()->IO.sys_projects_pools.empty()) {
        m_AssetFinder->m_TargetPossibilities = VortexMaker::GetCurrentContext()->IO.sys_modules_pools;
      }

      m_AssetFinder->GetAppWindow()->SetVisibility(true);
      m_AssetFinder->m_ItemToReconize.push_back(
          std::make_shared<AssetFinderItem>(
              VortexMaker::CheckProjectInDirectory,
              Cherry::GetLocale("loc.project"),
              Cherry::GetLocale("loc.project_tool"),
              Cherry::HexToRGBA("#B1FF31")));

      Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
    }

    CherryGUI::SameLine();
    Cherry::SetNextComponentProperty("padding_x", "8");
    Cherry::SetNextComponentProperty("padding_y", "4");
    if (show_filters) {
      Cherry::SetNextComponentProperty("color_bg", std::string(pal.accent));
      Cherry::SetNextComponentProperty("color_text", std::string(pal.accentText));
    }
    if (CherryKit::ButtonImageText(
            CherryID("open_search_toggle"), "", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"))
            .GetData("isClicked") == "true") {
      show_filters = !show_filters;
      if (!show_filters) {
        search[0] = '\0';
        type_filter = 0;
        version_filter.clear();
        sort_mode = 0;
      }
      refresh_frames = 1;
    }

    CherryGUI::SameLine(0.0f, 16.0f);
    if (chip("Grid", !list_view, 10.0f, 3.0f) && list_view) {
      list_view = false;
      refresh_frames = 1;
    }
    CherryGUI::SameLine(0.0f, 4.0f);
    if (chip("List", list_view, 10.0f, 3.0f) && !list_view) {
      list_view = true;
      refresh_frames = 1;
    }

    if (m_AssetFinder) {
      if (m_AssetFinder->m_GetFileBrowserPath) {
        m_AssetFinder->m_GetFileBrowserPath = false;
        // m_FindedModules.clear();

        std::string pool;
        if (!VortexMaker::GetCurrentContext()->IO.sys_projects_pools[m_AssetFinder->m_TargetPoolIndex].empty()) {
          pool = VortexMaker::GetCurrentContext()->IO.sys_projects_pools[m_AssetFinder->m_TargetPoolIndex];
        }

        for (auto selected : m_AssetFinder->m_Selected) {
          VortexMaker::ImportProject(selected, pool);
        }

        project_blocks.clear();
        VortexMaker::RefreshEnvironmentProjects();
        m_AssetFinder->GetAppWindow()->SetVisibility(false);
        m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
      }
    }

    const auto& all_projects = VortexMaker::GetCurrentContext()->IO.sys_projects;

    if (show_filters) {
      int count_project = 0, count_tool = 0;
      std::vector<std::string> versions;
      for (auto& e : all_projects) {
        if (!e)
          continue;
        if (e->type == "tool")
          count_tool++;
        else
          count_project++;
        if (!e->compatibleWith.empty() && std::find(versions.begin(), versions.end(), e->compatibleWith) == versions.end()) {
          versions.push_back(e->compatibleWith);
        }
      }
      std::sort(
          versions.begin(), versions.end(), [](const std::string& a, const std::string& b) { return VersionLess(b, a); });
      if (!version_filter.empty() && std::find(versions.begin(), versions.end(), version_filter) == versions.end()) {
        version_filter.clear();
        refresh_frames = 1;
      }

      CherryStyle::AddMarginY(6.0f);
      CherryStyle::AddMarginX(8.0f);

      CherryGUI::SetNextItemWidth(220.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
      CherryGUI::PushStyleColor(ImGuiCol_FrameBg, Cherry::HexToRGBA(pal.card));
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA(pal.border));
      CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.text));
      if (CherryGUI::InputTextWithHint("##open_search", "Search name, description, version...", search, sizeof(search))) {
        refresh_frames = 1;
      }

      CherryGUI::SameLine(0.0f, 10.0f);
      CherryGUI::SetNextItemWidth(130.0f);
      if (CherryGUI::BeginCombo("##open_version", version_filter.empty() ? "All versions" : version_filter.c_str())) {
        if (CherryGUI::Selectable("All versions", version_filter.empty())) {
          version_filter.clear();
          refresh_frames = 1;
        }
        for (auto& v : versions) {
          if (CherryGUI::Selectable(v.c_str(), version_filter == v)) {
            version_filter = v;
            refresh_frames = 1;
          }
        }
        CherryGUI::EndCombo();
      }

      CherryGUI::SameLine(0.0f, 6.0f);
      CherryGUI::SetNextItemWidth(110.0f);
      static const char* sort_labels[] = { "Recent", "Name", "Version" };
      if (CherryGUI::BeginCombo("##open_sort", sort_labels[sort_mode])) {
        for (int i = 0; i < 3; i++) {
          if (CherryGUI::Selectable(sort_labels[i], sort_mode == i)) {
            sort_mode = i;
            refresh_frames = 1;
          }
        }
        CherryGUI::EndCombo();
      }
      CherryGUI::PopStyleColor(3);
      CherryGUI::PopStyleVar(2);

      CherryGUI::SameLine(0.0f, 14.0f);
      if (chip("All (" + std::to_string(count_project + count_tool) + ")", type_filter == 0)) {
        type_filter = 0;
        refresh_frames = 1;
      }
      CherryGUI::SameLine(0.0f, 6.0f);
      if (chip(Cherry::GetLocale("loc.project") + " (" + std::to_string(count_project) + ")", type_filter == 1)) {
        type_filter = 1;
        refresh_frames = 1;
      }
      CherryGUI::SameLine(0.0f, 6.0f);
      if (chip(Cherry::GetLocale("loc.tool") + " (" + std::to_string(count_tool) + ")", type_filter == 2)) {
        type_filter = 2;
        refresh_frames = 1;
      }

      CherryGUI::NewLine();
    }

    CherryNextProp("color", pal.sep);
    CherryKit::Separator();

    const std::string query = LowerStr(search);
    std::vector<std::shared_ptr<EnvProject>> items;
    for (auto& e : all_projects) {
      if (!e)
        continue;
      const bool is_tool = (e->type == "tool");
      if (type_filter == 1 && is_tool)
        continue;
      if (type_filter == 2 && !is_tool)
        continue;
      if (!version_filter.empty() && e->compatibleWith != version_filter)
        continue;
      if (!query.empty() && LowerStr(e->name).find(query) == std::string::npos &&
          LowerStr(e->description).find(query) == std::string::npos &&
          LowerStr(e->compatibleWith).find(query) == std::string::npos) {
        continue;
      }
      items.push_back(e);
    }

    static const std::string kNever;
    const auto& last_opened = ProjectLastOpenedMap(all_projects);
    auto last_of = [&](const std::shared_ptr<EnvProject>& p) -> const std::string& {
      auto it = last_opened.find(p->path);
      return it != last_opened.end() ? it->second : kNever;
    };
    std::stable_sort(
        items.begin(), items.end(), [&](const std::shared_ptr<EnvProject>& a, const std::shared_ptr<EnvProject>& b) {
          switch (sort_mode) {
            case 1: return LowerStr(a->name) < LowerStr(b->name);
            case 2: return VersionLess(b->compatibleWith, a->compatibleWith);
            default: return last_of(a) > last_of(b);
          }
        });

    project_blocks.clear();

    const bool skip_frame = project_deleted || refresh_frames > 0;
    project_deleted = false;
    if (refresh_frames > 0)
      refresh_frames--;

    if (all_projects.empty()) {
      if (CherryKit::BlockVerticalCustom(
              []() { },
              100.0f,
              100.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/base/add.png"), 15, 15); },
                  []() { CherryKit::TextCenter("Create new"); },
              },
              200)
              .GetData("isClicked") == "true") {
        m_SelectedChildName = "?loc:loc.windows.welcome.create_project";
      }
    } else if (skip_frame) {
      // recreate item on incoming frame...
    } else if (items.empty()) {
      CherryKit::Space(30.0f);
      CherryNextProp("color_text", pal.sub);
      CherryKit::TextCenter("No project matches your filters.");
    } else if (!list_view) {
      int index = 0;
      for (auto element : items) {
        index++;
        CherryNextComponent.SetRenderMode(RenderMode::CreateOnly);
        project_blocks.push_back(
            CherryKit::BlockVerticalCustom(
                [this, element]() { m_SelectedEnvproject = element; },
                150.0f,
                115.0f,
                {
                    []() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 150.0f); },
                    [element]() {
                      CherryStyle::RemoveMarginY(35.0f);
                      CherryStyle::AddMarginX(10.0f);
                      CherryKit::ImageLocal(ProjectLogoPath(element->logoPath), 40.0f, 40.0f);
                    },
                    [element]() {
                      CherryStyle::AddMarginX(5.0f);
                      if (CherryApp.GetTheme() == "dark_vortex") {
                        CherryNextProp("color_text", "#FFFFFF");
                      } else {
                        CherryNextProp("color_text", "#232323");
                      }
                      CherryKit::TextSimple(element->name);
                    },
                    [element]() {
                      if (CherryApp.GetTheme() == "dark_vortex") {
                        Cherry::SetNextComponentProperty("color", "#353535");
                      } else {
                        Cherry::SetNextComponentProperty("color", "#898989");
                      }
                      CherryKit::Separator();
                      CherryStyle::AddMarginX(5.0f);

                      std::string versionpath;
                      bool version_available =
                          VortexMaker::CheckIfVortexVersionUtilityExist(element->compatibleWith, versionpath);
                      if (version_available) {
                        Cherry::SetNextComponentProperty("color_text", "#AAAAAA");
                      } else {
                        if (VortexMaker::CheckVersionAvailibility(element->compatibleWith).version == "") {
                          Cherry::SetNextComponentProperty("color_text", "#EE5555");
                        } else {
                          Cherry::SetNextComponentProperty("color_text", "#EEAA55");
                        }
                      }
                      CherryKit::TextSimple(element->compatibleWith);
                      if (!version_available) {
                        CherryGUI::SameLine();

                        if (VortexMaker::CheckVersionAvailibility(element->compatibleWith).version == "") {
                          CherryKit::TooltipImage(
                              Cherry::GetPath("resources/base/error.png"),
                              "Vortex " + element->compatibleWith + " " +
                                  Cherry::GetLocale("loc.windows.welcome.not_installed"));
                          CherryGUI::SameLine();
                          Cherry::SetNextComponentProperty("color_text", "#888888");
                        } else {
                          CherryKit::TooltipImage(
                              Cherry::GetPath("resources/base/warn.png"),
                              "Vortex " + element->compatibleWith + " " +
                                  Cherry::GetLocale("loc.windows.welcome.not_installed_yet"));
                          CherryGUI::SameLine();
                          Cherry::SetNextComponentProperty("color_text", "#888888");
                        }
                        CherryStyle::RemoveMarginX(5.0f);
                      }
                    },
                },
                index));
      }
      CherryKit::GridSimple(150.0f, 150.0f, project_blocks);
    } else {
      CherryGUI::BeginChild("###proj_list", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground);
      CherryStyle::AddMarginY(6.0f);

      const float row_h = 60.0f;
      int index = 0;

      for (auto element : items) {
        index++;
        const bool selected = (m_SelectedEnvproject == element);
        const float row_w = CherryGUI::GetContentRegionAvail().x - 14.0f;
        const int state = ProjectVersionState(element->compatibleWith);

        CherryStyle::AddMarginX(8.0f);
        ImVec2 p = CherryGUI::GetCursorScreenPos();
        const bool hovered = CherryGUI::IsMouseHoveringRect(p, ImVec2(p.x + row_w, p.y + row_h));
        bool row_clicked = false;

        CherryGUI::PushID(index);
        CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA(hovered ? pal.cardHover : pal.card));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA(selected ? pal.accent : pal.border));
        CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, selected ? 2.0f : 1.0f);

        if (CherryGUI::BeginChild(
                "##proj_row",
                ImVec2(row_w, row_h),
                true,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
          // Logo
          CherryGUI::SetCursorPos(ImVec2(12.0f, (row_h - 38.0f) * 0.5f));
          CherryKit::ImageLocal(ProjectLogoPath(element->logoPath), 38.0f, 38.0f);

          const float name_w = std::max(180.0f, row_w * 0.36f);
          CherryGUI::SameLine(0.0f, 12.0f);
          CherryGUI::BeginGroup();
          CherryGUI::SetCursorPosY(10.0f);
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", pal.text);
          CherryKit::TitleSix(element->name);
          Cherry::PopFont();
          CherryNextProp("color_text", pal.sub);
          CherryKit::TextSimple(element->description);
          CherryGUI::EndGroup();

          const float type_x = 12.0f + 38.0f + 12.0f + name_w;
          CherryGUI::SetCursorPos(ImVec2(type_x, (row_h - CherryGUI::GetTextLineHeight() - 4.0f) * 0.5f));
          const bool is_tool = (element->type == "tool");
          CreatePill(is_tool ? Cherry::GetLocale("loc.tool") : Cherry::GetLocale("loc.project"), pal.pillBg, pal.pillText);

          CherryGUI::SetCursorPos(ImVec2(type_x + 100.0f, (row_h - CherryGUI::GetTextLineHeight() - 4.0f) * 0.5f));
          CreatePill(element->compatibleWith, pal.pillBg, ProjectStateColor(state, pal));
          if (state != 0) {
            CherryGUI::SameLine(0.0f, 6.0f);
            if (state == 2) {
              CherryKit::TooltipImage(
                  Cherry::GetPath("resources/base/error.png"),
                  "Vortex " + element->compatibleWith + " " + Cherry::GetLocale("loc.windows.welcome.not_installed"));
            } else {
              CherryKit::TooltipImage(
                  Cherry::GetPath("resources/base/warn.png"),
                  "Vortex " + element->compatibleWith + " " + Cherry::GetLocale("loc.windows.welcome.not_installed_yet"));
            }
          }

          CherryGUI::SetCursorPos(ImVec2(row_w - 200.0f, (row_h - 24.0f) * 0.5f));
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "5");
          if (CherryKit::ButtonImageText(
                  CherryID("proj_delete" + std::to_string(index)),
                  Cherry::GetLocale("loc.delete"),
                  Cherry::GetPath("resources/imgs/trash.png"))
                  .GetData("isClicked") == "true") {
            do_delete(element);
          }
          CherryGUI::SameLine(0.0f, 8.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "5");
          Cherry::SetNextComponentProperty("color_bg", std::string(pal.accent));
          Cherry::SetNextComponentProperty("color_text", std::string(pal.accentText));
          if (CherryKit::ButtonImageText(
                  CherryID("proj_open" + std::to_string(index)),
                  Cherry::GetLocale("loc.open"),
                  Cherry::GetPath("resources/imgs/open.png"))
                  .GetData("isClicked") == "true") {
            m_SelectedEnvproject = element;
            do_open(element);
          }

          if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseClicked(0))
            row_clicked = true;
        }
        CherryGUI::EndChild();

        CherryGUI::PopStyleVar(2);
        CherryGUI::PopStyleColor(2);
        CherryGUI::PopID();

        if (row_clicked)
          m_SelectedEnvproject = element;
        CherryStyle::AddMarginY(4.0f);
      }
      CherryGUI::EndChild();  // ###proj_list
    }

    CherryGUI::EndChild();  // left_pane

    if (CherryApp.GetTheme() == "dark_vortex") {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#DFDFDF"));
    }
    CherryGUI::BeginChild("###rightpan", ImVec2(0, bottom_pan), false, ImGuiWindowFlags_NoScrollbar);
    if (!m_SelectedEnvproject) {
      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;

      CherryStyle::AddMarginY(7.0f);
      CherryGUI::BeginChild('SADH', ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
      {
        CherryStyle::AddMarginX(8.0f);
        MyButton(Cherry::GetPath("resources/imgs/select.png"), 35, 35);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        CherryStyle::RemoveMarginY(10.0f);
        ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
        CherryGUI::BeginChild(_id, ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.select_a_project"));
        CherryGUI::EndChild();
      }
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#44444466"));

      ImVec2 child_size = CherryGUI::GetContentRegionAvail();
      float imagex = child_size.x;
      float imagey = imagex / 3.235;

      CherryStyle::AddMarginY(7.0f);
      CherryGUI::BeginChild('SADH', ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
      {
        CherryStyle::AddMarginX(8.0f);
        MyButton(m_SelectedEnvproject->logoPath, 35, 35);
        CherryGUI::SameLine();
        CherryStyle::AddMarginY(20.0f);
      }

      {
        CherryStyle::RemoveMarginY(10.0f);
        ImGuiID _id = CherryGUI::GetID("INFO_PANEL");
        CherryGUI::BeginChild(_id, ImVec2(0, 35), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        CherryKit::TitleSix(m_SelectedEnvproject->name);
        CherryGUI::EndChild();
      }

      // Divider
      CherryStyle::RemoveMarginY(15.0f);
      CherryGUI::Separator();
      Cherry::SetNextComponentProperty("color_text", "#B1FF31");

      std::string project_type;
      if (m_SelectedEnvproject->type == "project") {
        project_type = Cherry::GetLocale("loc.project");
      } else if (m_SelectedEnvproject->type == "tool") {
        project_type = Cherry::GetLocale("loc.tool");
      } else {
        project_type = Cherry::GetLocale("loc.project");
      }

      CherryGUI::BeginHorizontal("InfoBar");

      CherryStyle::AddMarginX(5.0f);
      CherryKit::TitleSix(project_type);

      ImVec2 p1 = ImVec2(CherryGUI::GetCursorScreenPos().x, CherryGUI::GetCursorScreenPos().y - 5.0f);
      ImVec2 p2 = ImVec2(p1.x, p1.y + 25);
      CherryGUI::GetWindowDrawList()->AddLine(p1, p2, Cherry::HexToImU32("#343434"));
      CherryGUI::Dummy(ImVec2(1, 0));

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryKit::TitleSix(m_SelectedEnvproject->compatibleWith);

      CherryGUI::EndHorizontal();

      CherryGUI::BeginChild("aboutchild", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground);  // cherry api

      Cherry::SetNextComponentProperty("color_text", "#585858");
      CherryStyle::AddMarginX(5.0f);
      CherryKit::TextSimple(m_SelectedEnvproject->description);

      std::string text = Cherry::GetLocale("loc.delete") + " " + Cherry::GetLocale("loc.open");
      ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
      CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);

      CherryNextProp("color_text", "#B1FF31");
      if (CherryKit::ButtonText(Cherry::GetLocale("loc.delete")).GetData("isClicked") == "true") {
        m_SelectedEnvprojectToRemove = m_SelectedEnvproject;
        open_deletion_modal = true;
      }

      CherryGUI::SameLine();

      Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
      Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
      Cherry::SetNextComponentProperty("color_text", "#121212FF");
      if (CherryKit::ButtonText(Cherry::GetLocale("loc.open")).GetData("isClicked") == "true") {
        std::string versionpath;
        bool version_exist =
            VortexMaker::CheckIfVortexVersionUtilityExist(m_SelectedEnvproject->compatibleWith, versionpath);
        if (!version_exist) {
          no_installed_version = m_SelectedEnvproject->compatibleWith;
          no_installed_project_name = m_SelectedEnvproject->name;
          no_installed_project_picture = m_SelectedEnvproject->logoPath;

          no_installed_version_available = VortexMaker::CheckVersionAvailibility(m_SelectedEnvproject->compatibleWith);

          no_installed_modal_opened = true;
        } else {
          RequestOpen(m_SelectedEnvproject);
        }
      }
      CherryGUI::EndChild();

      ImVec2 windowSize = CherryGUI::GetWindowSize();
      ImVec2 windowPos = CherryGUI::GetWindowPos();
      ImVec2 buttonSize = ImVec2(120, 35);
      float footerHeight = buttonSize.y + CherryGUI::GetStyle().ItemSpacing.y * 2;

      CherryGUI::SetCursorPosY(windowSize.y - footerHeight - 10.0f);

      CherryGUI::Separator();
      CherryGUI::Spacing();

      CherryGUI::EndChild();

      CherryGUI::PopStyleColor();
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void WelcomeWindow::WelcomeRender() {
    if (VortexMaker::GetCurrentContext()->web_fetched) {
      CherryNextComponent.SetProperty("color_border", "#343434");
      CherryNextComponent.SetProperty("color_bg", "#232323");
      CherryKit::NotificationButton(
          &VortexMaker::GetCurrentContext()->launcher_update_available,
          10,
          "info",
          Cherry::GetLocale("loc.windows.welcome.update_launcher_title"),
          Cherry::GetLocale("loc.windows.welcome.update_launcher_description") +
              VortexMaker::GetCurrentContext()->latest_launcher_version.version,
          []() {
            if (CherryKit::ButtonImageText(
                    Cherry::GetLocale("loc.update_now"), Cherry::GetPath("resources/imgs/icons/misc/icon_upgrade.png"))
                    .GetData("isClicked") == "true") {
              std::thread([]() {
                VortexMaker::OpenLauncherUpdater(
                    VortexMaker::GetCurrentContext()->m_VortexLauncherPath,
                    VortexMaker::GetCurrentContext()->IO.sys_vortexlauncher_dist);
              }).detach();
              Cherry::Application::Get().Close();
            }
          });

      if (VortexMaker::GetCurrentContext()->latest_vortex_version) {
        CherryKit::NotificationSimple(
            &VortexMaker::GetCurrentContext()->vortex_update_available,
            10,
            "info",
            "Vortex " + VortexMaker::GetCurrentContext()->latest_vortex_version->version + " " +
                Cherry::GetLocale("loc.is_live"),
            Cherry::GetLocale("loc.menubar.menuitem.install_vortex_desc") +
                VortexMaker::GetCurrentContext()->latest_vortex_version->version);
      }
    }

    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    {
      float x = CherryGUI::GetContentRegionAvail().x;
      float y = x / 4.726f;
      if (CherryApp.GetTheme() == "dark_vortex") {
        CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/vortex_banner.png"), x, y);
      } else {
        CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/vortex_banner_light.png"), x, y);
      }
      CherryStyle::AddMarginX(20.0f);
      CherryStyle::RemoveMarginY(60.0f);
      Cherry::PushFont("ClashMedium");
      if (CherryApp.GetTheme() == "dark_vortex") {
        CherryNextProp("color_text", "#FFFFFF");
      } else {
        CherryNextProp("color_text", "#232323");
      }
      CherryKit::TitleOne(Cherry::GetLocale("loc.windows.welcome.title"));
      Cherry::PopFont();
    }

    /*Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#BBBBBB");
    CherryKit::TitleOne(Cherry::GetLocale("loc.windows.welcome.overview"));
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryNextProp("color", "#252525");
    CherryKit::Separator();*/

    CherryKit::Space(25.0f);

    // CherryStyle::AddMarginY(20.0f);
    // CherryStyle::AddMarginX(8.0f);
    // CherryNextProp("color_text", "#797979");
    // CherryKit::TitleFour("Fast actions");

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.latest_projects_tools"));
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> blocks;

    if (blocks.empty()) {
      auto recentProjects = RecentProjectsBySession(4);

      int i = 0;
      for (auto project : recentProjects) {
        if (project) {
          CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
          blocks.push_back(
              CherryKit::BlockVerticalCustom(
                  [=]() { m_ProjectCallback(project); },
                  200.0f,
                  120.0f,
                  {
                      [=]() { CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/def_project_banner.png"), 200, 75); },
                      [=]() {
                        CherryStyle::AddMarginX(5.0f);
                        CherryKit::TitleSix(project->name);
                      },
                      [=]() {
                        CherryStyle::AddMarginX(5.0f);
                        CherryStyle::RemoveMarginY(5.0f);
                        CherryStyle::PushFontSize(0.70f);
                        CherryKit::TextSimple(FormatLastOpened(project->path));
                        CherryStyle::PopFontSize();
                      },
                  },
                  i));
        }

        i++;
      }

      while (i < 4) {
        std::string str1;
        std::string str2;
        if (i == 0) {
          str1 = Cherry::GetLocale("loc.windows.welcome.no_project_yet");
          str2 = Cherry::GetLocale("loc.windows.welcome.create_one");
        } else {
          str1 = Cherry::GetLocale("loc.windows.welcome.no_more_project");
          str2 = Cherry::GetLocale("loc.windows.welcome.create_another");
        }
        CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
        blocks.push_back(
            CherryKit::BlockVerticalCustom(
                m_CreateProjectCallback,
                200.0f,
                120.0f,
                {
                    [=]() {
                      CherryKit::Space(40.0f);
                      CherryStyle::PushFontSize(0.8f);
                      CherryNextProp("color_text", "#787878");
                      CherryKit::TextCenter(str1);
                      CherryStyle::PopFontSize();
                      CherryNextProp("color_text", "#787878");
                      CherryStyle::PushFontSize(0.6f);
                      CherryKit::TextCenter(str2);
                      CherryStyle::PopFontSize();
                    },
                },
                i));
        i++;
      }
    }

    CherryStyle::AddMarginX(8.0f);
    CherryKit::GridSimple(200.0f, 200.0f, blocks);

    CherryKit::Space(5.0f);

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.fast_actions"));
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> actions_blocks;
    std::vector<Cherry::Component> actions_blocks_bottom;

    if (actions_blocks.empty()) {
      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              [this]() { m_SelectedChildName = "?loc:loc.windows.welcome.create_project"; },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/add.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    if (CherryApp.GetTheme() == "dark_vortex") {
                      CherryNextProp("color_text", "#FFFFFF");
                    } else {
                      CherryNextProp("color_text", "#232323");
                    }
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.create_a_project"));
                  },
              },
              1));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              [this]() { m_SelectedChildName = "?loc:loc.windows.welcome.open_project"; },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/open.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    if (CherryApp.GetTheme() == "dark_vortex") {
                      CherryNextProp("color_text", "#FFFFFF");
                    } else {
                      CherryNextProp("color_text", "#232323");
                    }
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.open_a_project"));
                  },
              },
              2));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks.push_back(
          CherryKit::BlockVerticalCustom(
              m_SettingsCallback,
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/settings.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    if (CherryApp.GetTheme() == "dark_vortex") {
                      CherryNextProp("color_text", "#FFFFFF");
                    } else {
                      CherryNextProp("color_text", "#232323");
                    }
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.settings_confs"));
                  },
              },
              3));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks_bottom.push_back(
          CherryKit::BlockVerticalCustom(
              []() { VortexMaker::OpenURL("https://vortex.infinite.si/learn"); },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/icons/launcher/docs.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    if (CherryApp.GetTheme() == "dark_vortex") {
                      CherryNextProp("color_text", "#FFFFFF");
                    } else {
                      CherryNextProp("color_text", "#232323");
                    }
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.learn_docs"));
                  },
              },
              4));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks_bottom.push_back(
          CherryKit::BlockVerticalCustom(
              []() { VortexMaker::OpenURL("https://forums.infinite.si/"); },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() {
                    CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/icons/launcher/forum.png"), 30, 30);
                  },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    if (CherryApp.GetTheme() == "dark_vortex") {
                      CherryNextProp("color_text", "#FFFFFF");
                    } else {
                      CherryNextProp("color_text", "#232323");
                    }
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.forums"));
                  },
              },
              5));

      CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
      actions_blocks_bottom.push_back(
          CherryKit::BlockVerticalCustom(
              []() { VortexMaker::OpenURL("https://vortex.infinite.si/news"); },
              269.0f,
              120.0f,
              {
                  []() { CherryKit::Space(20.0f); },
                  []() { CherryKit::ImageLocalCentered(Cherry::GetPath("resources/imgs/icons/launcher/web.png"), 30, 30); },
                  []() { CherryKit::Space(15.0f); },
                  []() {
                    if (CherryApp.GetTheme() == "dark_vortex") {
                      CherryNextProp("color_text", "#FFFFFF");
                    } else {
                      CherryNextProp("color_text", "#232323");
                    }
                    CherryKit::TextCenter(Cherry::GetLocale("loc.windows.welcome.whats_news"));
                  },
              },
              6));
    }

    // Draw grid with blocks
    CherryStyle::AddMarginX(8.0f);
    CherryKit::GridSimple(270.0f, 270.0f, actions_blocks);
    CherryStyle::AddMarginX(8.0f);
    CherryKit::GridSimple(270.0f, 270.0f, actions_blocks_bottom);

    CherryKit::Space(8.0f);
    CherryNextProp("color", "#222222");

    Cherry::PushFont("ClashMedium");
    CherryNextProp("color_text", "#676767");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::TitleSix(Cherry::GetLocale("loc.windows.welcome.latest_news"));
    Cherry::PopFont();
    CherryNextProp("color", "#222222");
    CherryStyle::AddMarginX(8.0f);
    CherryKit::Separator();

    std::vector<Cherry::Component> news_blocks;

    static bool was_disconnected;
    static bool refresh_grid_needed = false;

    if (!VortexMaker::GetCurrentContext()->web_fetched) {
      was_disconnected = true;
      if (news_blocks.empty()) {
        CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
        auto block = CherryKit::BannerImageContext(
            402.0f, 140.0f, Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"), "", "");
        news_blocks.push_back(block);
        news_blocks.push_back(block);
      }
    } else {
      if (was_disconnected) {
        news_blocks.clear();
        was_disconnected = false;
        refresh_grid_needed = true;
      }

      if (news_blocks.empty()) {
        for (const auto& article : VortexMaker::GetCurrentContext()->IO.news) {
          if (!article.image_link.empty() &&
              (EndsWith(article.image_link, ".png") || EndsWith(article.image_link, ".jpg")) &&
              (article.image_link.find("http://") == 0 || article.image_link.find("https://") == 0)) {
            CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
            auto block = CherryKit::BannerImageContext(
                402.0f, 140.0f, Cherry::GetHttpPath(article.image_link), article.title, article.description);
            news_blocks.push_back(block);
          }
        }
      }
    }

    CherryStyle::AddMarginX(8.0f);

    if (!refresh_grid_needed) {
      CherryKit::GridSimple(400.0f, 400.0f, news_blocks);
    } else {
      refresh_grid_needed = false;
    }

    /*CherryKit::Space(20.0f);
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleThree("Latest Vortex versions");
    Cherry::PopFont();
    CherryKit::Separator();

    static std::vector<std::shared_ptr<Cherry::Component>> last_versions_blocks;

    if (last_versions_blocks.empty()) {
      int version_index = 0;
      for (auto version : VortexMaker::GetCurrentContext()->latest_vortex_versions) {
        if (version_index < 3) {
          auto block = CherryKit::BlockVerticalCustom(
              Cherry::IdentifierProperty::CreateOnly,
              [=]() {},
              200.0f,
              120.0f,
              {
                  [=]() { CherryKit::ImageHttp(version.banner, 260, 75); },
                  [=]() {
                    CherryStyle::AddMarginX(5.0f);
                    CherryKit::TitleSix(version.name);
                  },
                  /*CherryLambda
                  (
                      CherryStyle::AddMarginX(5.0f);
                      CherryStyle::RemoveMarginY(5.0f);
                      CherryStyle::PushFontSize(0.70f);
                      CherryKit::TextSimple(version.already_installed);
                      CherryStyle::PopFontSize();
                  ),
  });
  last_versions_blocks.push_back(block);

  version_index++;
}
}
}

CherryKit::GridSimple(150.0f, 150.0f, &last_versions_blocks);
*/
  }

  WelcomeWindow::WelcomeWindow(const std::string& name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));
    m_AppWindow->SetClosable(false);

    m_AppWindow->m_TabMenuCallback = []() {
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    m_SelectedChildName = "?loc:loc.windows.welcome.overview";
    m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);
    RefreshTemplates();
    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
    loadProjects(projectPoolsPaths, path + "/projects_pools.json");

    /*   this->AddChild(
           "Support Us",
           WelcomeWindowChild(
               [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/heart.png"), "https://fund.infinite.si/"));
       this->AddChild(
           "What's news",
           WelcomeWindowChild(
               [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/web.png"), "https://vortex.infinite.si/news"));
       this->AddChild(
           "Forums",
           WelcomeWindowChild(
               [this]() { }, Cherry::GetPath("resources/imgs/icons/launcher/forum.png"),
       "https://forums.infinite.si/vortex")); this->AddChild( "Documentation", WelcomeWindowChild( [this]() { },
       Cherry::GetPath("resources/imgs/icons/launcher/docs.png"), "https://vortex.infinite.si/learn"));
   */
    this->AddChild(
        "?loc:loc.windows.welcome.overview",
        WelcomeWindowChild(
            [this]() { this->WelcomeRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));
    this->AddChild(
        "?loc:loc.windows.welcome.open_project",
        WelcomeWindowChild(
            [this]() { this->OpenProjectRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));
    this->AddChild(
        "?loc:loc.windows.welcome.create_project",
        WelcomeWindowChild(
            [this]() { this->CreateProjectRender(); }, Cherry::GetPath("resources/imgs/icons/launcher/overview.png")));

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::vector<std::shared_ptr<EnvProject>> WelcomeWindow::GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>>& projects,
      size_t maxCount) {
    auto sortedProjects = projects;
    std::sort(
        sortedProjects.begin(),
        sortedProjects.end(),
        [](const std::shared_ptr<EnvProject>& a, const std::shared_ptr<EnvProject>& b) {
          return a->lastOpened > b->lastOpened;
        });

    if (sortedProjects.size() > maxCount) {
      sortedProjects.resize(maxCount);
    }
    return sortedProjects;
  }

  void WelcomeWindow::AddChild(const std::string& child_name, const WelcomeWindowChild& child) {
    m_Childs[child_name] = child;
  }

  void WelcomeWindow::RemoveChild(const std::string& child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow>& WelcomeWindow::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<WelcomeWindow> WelcomeWindow::Create(const std::string& name) {
    auto instance = std::shared_ptr<WelcomeWindow>(new WelcomeWindow(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void WelcomeWindow::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  WelcomeWindowChild* WelcomeWindow::GetChild(const std::string& child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void WelcomeWindow::LaunchSelectedProject() {
    auto sys_versions = VortexMaker::GetAllSystemVersions(m_SelectedEnvproject->compatibleWith);
    if (sys_versions.size() == 1) {
      std::thread([path = m_SelectedEnvproject->path, version = sys_versions.front()->name]() {
        VortexMaker::OpenProject(path, version);
      }).detach();
    } else {
      all_versions_for_project = sys_versions;
      selected_version_index = 0;
      const std::string favorite = sessions::GetFavoriteVersion(m_SelectedEnvproject->path);
      for (size_t i = 0; i < sys_versions.size(); ++i) {
        if (sys_versions[i]->name == favorite) {
          selected_version_index = static_cast<int>(i);
          break;
        }
      }
      multiple_versions_modal_opened = true;
    }
  }

  void WelcomeWindow::RequestOpen(std::shared_ptr<EnvProject> project) {
    m_SelectedEnvproject = std::move(project);
    if (sessions::IsProjectOpen(m_SelectedEnvproject->path)) {
      already_running_modal_opened = true;
      return;
    }
    LaunchSelectedProject();
  }

  void WelcomeWindow::Render() {
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;

    std::string label = "left_pane" + m_AppWindow->m_Name;
    if (CherryApp.GetTheme() == "dark_vortex") {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#FFFFFF"));
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#EDEDED"));
    }
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, NULL);

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);

    const float input_width = leftPaneWidth - 17.0f;
    const float header_width = leftPaneWidth - 27.0f;

    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));

    CherryKit::Space(3.0f);

    if (CherryApp.GetTheme() == "dark_vortex") {
      // if selected
      if (m_SelectedChildName == "?loc:loc.windows.welcome.overview") {
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#23232300"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#34343400"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      }

    } else {
      if (m_SelectedChildName == "?loc:loc.windows.welcome.overview") {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFF"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#DFDFDF99"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#DFDFDF55"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#FFFFFF"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFF00"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#DFDFDF00"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#DFDFDF55"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#FFFFFF"));
      }
    }

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.overview").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      m_SelectedChildName = "?loc:loc.windows.welcome.overview";
    }
    CherryGUI::PopStyleColor(4);

    if (CherryApp.GetTheme() == "dark_vortex") {
      // if selected
      if (m_SelectedChildName == "?loc:loc.windows.welcome.create_project") {
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#23232300"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#34343400"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      }

    } else {
      if (m_SelectedChildName == "?loc:loc.windows.welcome.create_project") {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFF"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#DFDFDF99"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#DFDFDF55"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#FFFFFF"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFF00"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#DFDFDF00"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#DFDFDF55"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#FFFFFF"));
      }
    }

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.create_projects").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      m_SelectedChildName = "?loc:loc.windows.welcome.create_project";
    }
    CherryGUI::PopStyleColor(4);

    if (CherryApp.GetTheme() == "dark_vortex") {
      // if selected
      if (m_SelectedChildName == "?loc:loc.windows.welcome.open_project") {
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#23232300"));
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#34343400"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      }

    } else {
      if (m_SelectedChildName == "?loc:loc.windows.welcome.open_project") {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFF"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#DFDFDF99"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#DFDFDF55"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#FFFFFF"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FFFFFF00"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#DFDFDF00"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#DFDFDF55"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#FFFFFF"));
      }
    }

    CherryStyle::AddMarginX(6.0f);
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
            header_width,
            Cherry::GetLocale("loc.windows.welcome.your_projects").c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      m_SelectedChildName = "?loc:loc.windows.welcome.open_project";
    }
    CherryGUI::PopStyleColor(4);

    CherryStyle::AddMarginY(CherryGUI::GetContentRegionMax().y - 170.0f);
    CherryStyle::AddMarginX(6.0f);
    if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
            .GetDataAs<bool>("isClicked")) {
      if (m_SettingsCallback) {
        m_SettingsCallback();
      }
    }
    CherryGUI::PopStyleVar();

    // CherryStyle::SetPadding(7.0f);

    /*  for (const auto &child : m_Childs) {
        if (child.first == m_SelectedChildName) {
          // opt.hex_text_idle = "#FFFFFFFF";
        } else {
          // opt.hex_text_idle = "#A9A9A9FF";
        }
        std::string child_name;

        if (child.first.rfind("?loc:", 0) == 0) {
          std::string localeName = child.first.substr(5);
          child_name = Cherry::GetLocale(localeName) + "####" + localeName;
        } else {
          child_name = child.first;
        }

        if (child.second.WebLink == "undefined") {
          CherryNextProp("color_bg", "#00000000");
          CherryNextProp("color_border", "#00000000");
          CherryNextProp("padding_x", "2");
          CherryNextProp("padding_y", "2");
          CherryNextProp("size_x", "20");
          CherryNextProp("size_y", "20");
          CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
          CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath);
        } else {
          CherryNextProp("color_bg", "#00000000");
          CherryNextProp("color_border", "#00000000");
          CherryNextProp("padding_x", "2");
          CherryNextProp("padding_y", "2");
          CherryNextProp("size_x", "20");
          CherryNextProp("size_y", "20");
          CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
          if (CherryKit::ButtonImageTextImage(
                  CherryID(child_name),
                  child_name.c_str(),
                  child.second.LogoPath,
                  Cherry::GetPath("resources/imgs/weblink.png"))
                  .GetData("isClicked") == "true") {
            VortexMaker::OpenURL(child.second.WebLink);
          }
        }

        // if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
      }
  */

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(4);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    // CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);
    static std::string prev_child;
    static std::unordered_map<std::string, float> scroll_memory;

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

      ImGuiWindowFlags child_flags = 0;

      if (m_SelectedChildName == "?loc:loc.windows.welcome.open_project") {
        child_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
      } else {
        child_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
      }

      if (CherryGUI::BeginChild("ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | child_flags)) {
        if (prev_child != m_SelectedChildName) {
          if (!prev_child.empty()) {
            scroll_memory[prev_child] = CherryGUI::GetScrollY();
          }

          if (m_SelectedChildName == "?loc:loc.windows.welcome.open_project") {
            CherryGUI::SetScrollY(0.0f);
          }

          else if (m_SelectedChildName == "?loc:loc.windows.welcome.overview") {
            auto it = scroll_memory.find(m_SelectedChildName);
            if (it != scroll_memory.end()) {
              CherryGUI::SetScrollY(it->second);
            }
          }
        }

        if (auto child = GetChild(m_SelectedChildName)) {
          if (child->RenderCallback) {
            child->RenderCallback();
          }
        }
      }
      CherryGUI::EndChild();

      CherryGUI::PopStyleVar(2);

      prev_child = m_SelectedChildName;
    }

    CherryGUI::EndGroup();

    static bool user_string_validation = false;
    static std::string string_validation = "";
    if (open_deletion_modal) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.delete_project").c_str());

      if (CherryGUI::BeginPopupModal(Cherry::GetLocale("loc.windows.welcome.delete_project").c_str(), NULL, NULL)) {
        static char path_input_all[512];
        std::string text = CherryApp.GetLocale("loc.delete") + CherryApp.GetLocale("loc.close");
        ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());

        Cherry::SetNextComponentProperty("color_text", "#CC2222");
        CherryKit::TitleThree(m_SelectedEnvprojectToRemove->name);
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.project_delete_warning").c_str());
        CherryKit::Space(15.0f);
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.project_delete_warning_description").c_str());
        CherryKit::Space(15.0f);

        CherryNextComponent.SetProperty(
            "description", Cherry::GetLocale("loc.windows.welcome.project_delete_warning_button"));
        CherryNextComponent.SetProperty("description_logo", GetPath("resources/imgs/icons/misc/icon_trash.png"));
        CherryNextComponent.SetProperty("description_logo_place", "r");
        CherryNextComponent.SetProperty("size_x", CherryGUI::GetContentRegionMax().x - to_remove.x - 75);
        CherryKit::InputString("####name_validation", &string_validation);

        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
        CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 30.0f);
        CherryNextProp("color", "#222222");
        CherryKit::Separator();
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 40);

        CherryNextProp("color_text", "#B1FF31");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.close")).GetData("isClicked") == "true") {
          open_deletion_modal = false;
          CherryGUI::CloseCurrentPopup();
        }

        CherryGUI::SameLine();
        Cherry::SetNextComponentProperty("color_bg", "#ed5247");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#eda49f");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");

        if (strcmp(string_validation.c_str(), m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
          CherryGUI::BeginDisabled();
        }

        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.delete")).GetData("isClicked") == "true") {
          VortexMaker::DeleteProject(m_SelectedEnvprojectToRemove->path, m_SelectedEnvprojectToRemove->name);

          VortexMaker::RefreshEnvironmentProjects();
          project_deleted = true;

          open_deletion_modal = false;
          CherryGUI::CloseCurrentPopup();
        }

        if (strcmp(string_validation.c_str(), m_SelectedEnvprojectToRemove->name.c_str()) != 0) {
          CherryGUI::EndDisabled();
        }

        CherryGUI::EndPopup();
      }
    }

    if (already_running_modal_opened) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.project_already_running").c_str());

      ImVec2 main_window_size = CherryGUI::GetWindowSize();
      ImVec2 window_pos = CherryGUI::GetWindowPos();

      CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

      CherryGUI::SetNextWindowSize(ImVec2(400, 170), ImGuiCond_Always);
      if (CherryGUI::BeginPopupModal(
              Cherry::GetLocale("loc.windows.welcome.project_already_running").c_str(),
              NULL,
              ImGuiWindowFlags_AlwaysAutoResize)) {
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.project_already_running_description").c_str());

        std::string text = CherryApp.GetLocale("loc.open") + CherryApp.GetLocale("loc.close");
        ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
        CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 35.0f);
        CherryNextProp("color", "#222222");
        CherryKit::Separator();
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 40);

        CherryNextProp("color_text", "#B1FF31");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.close")).GetData("isClicked") == "true") {
          CherryGUI::CloseCurrentPopup();
          already_running_modal_opened = false;
        }

        CherryGUI::SameLine();

        Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.open")).GetData("isClicked") == "true") {
          LaunchSelectedProject();
          CherryGUI::CloseCurrentPopup();
          already_running_modal_opened = false;
        }

        CherryGUI::EndPopup();
      }
    }
    if (multiple_versions_modal_opened) {
      CherryGUI::OpenPopup(Cherry::GetLocale("loc.windows.welcome.select_a_verson").c_str());

      ImVec2 main_window_size = CherryGUI::GetWindowSize();
      ImVec2 window_pos = CherryGUI::GetWindowPos();

      CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 200, window_pos.y + 150));

      CherryGUI::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
      if (CherryGUI::BeginPopupModal(
              Cherry::GetLocale("loc.windows.welcome.select_a_verson").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        CherryGUI::TextWrapped(Cherry::GetLocale("loc.windows.welcome.select_a_verson_description").c_str());

        if (all_versions_for_project.empty()) {
          CherryGUI::Text(Cherry::GetLocale("loc.windows.welcome.no_versions_available").c_str());
        } else {
          std::vector<const char*> version_names;
          version_names.reserve(all_versions_for_project.size());

          for (const auto& v : all_versions_for_project) {
            version_names.push_back(v->name.c_str());
          }

          CherryGUI::Combo("##VersionSelector", &selected_version_index, version_names.data(), version_names.size());
        }

        CherryGUI::Spacing();

        std::string text = CherryApp.GetLocale("loc.open") + CherryApp.GetLocale("loc.close");
        ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
        CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 35.0f);
        CherryNextProp("color", "#222222");
        CherryKit::Separator();
        CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 40);

        CherryNextProp("color_text", "#B1FF31");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.close")).GetData("isClicked") == "true") {
          CherryGUI::CloseCurrentPopup();
          multiple_versions_modal_opened = false;
        }

        CherryGUI::SameLine();

        Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");
        if (CherryKit::ButtonText(CherryApp.GetLocale("loc.open")).GetData("isClicked") == "true") {
          if (!all_versions_for_project.empty()) {
            auto selected_version = all_versions_for_project[selected_version_index];
            sessions::SetFavoriteVersion(m_SelectedEnvproject->path, selected_version->name);
            std::thread([path = m_SelectedEnvproject->path, version = selected_version->name]() {
              VortexMaker::OpenProject(path, version);
            }).detach();
          }

          CherryGUI::CloseCurrentPopup();
          multiple_versions_modal_opened = false;
        }

        CherryGUI::EndPopup();
      }
    }

    if (no_installed_modal_opened) {
      const UiPalette pal = GetCreatePalette();
      const std::string popup_title = Cherry::GetLocale("loc.windows.welcome.no_version");

      CherryGUI::OpenPopup(popup_title.c_str());

      ImVec2 main_window_size = CherryGUI::GetWindowSize();
      ImVec2 window_pos = CherryGUI::GetWindowPos();

      CherryGUI::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 220, window_pos.y + 150));
      CherryGUI::SetNextWindowSize(ImVec2(440, 0), ImGuiCond_Always);

      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 18.0f));
      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, Cherry::HexToRGBA(pal.card));
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA(pal.border));
      CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA(pal.sep));

      if (CherryGUI::BeginPopupModal(
              popup_title.c_str(),
              NULL,
              ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", pal.text);
        CherryKit::TitleFour(popup_title);
        Cherry::PopFont();

        {
          std::string text_label = Cherry::GetLocale("loc.windows.welcome.no_version_1") + " \"" +
                                   no_installed_project_name + "\"" + Cherry::GetLocale("loc.windows.welcome.no_version_2") +
                                   +"\"" + no_installed_version + "\"" +
                                   Cherry::GetLocale("loc.windows.welcome.no_version_1");
          CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.sub));
          CherryGUI::TextWrapped("%s", text_label.c_str());
          CherryGUI::PopStyleColor();
        }
        CherryGUI::Spacing();
        CherryGUI::Separator();
        CherryGUI::Spacing();

        if (no_installed_version_available.version != "") {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", pal.text);
          CherryKit::TitleFive(Cherry::GetLocale("loc.all_projects"));
          Cherry::PopFont();

          {
            CherryGUI::BeginChild(
                "LOGO_", ImVec2(130, 42), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
            MyButton(Cherry::GetHttpPath(no_installed_version_available.banner), 120, 40);
            CherryGUI::EndChild();
            CherryGUI::SameLine();
          }
          {
            CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA(pal.panel));
            CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA(pal.border));
            CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
            CherryGUI::BeginChild(CherryGUI::GetID("INFO_PANEL"), ImVec2(0, 52), true, ImGuiWindowFlags_NoScrollbar);
            CherryGUI::SetCursorPosY(CherryGUI::GetStyle().ItemSpacing.y + 2.0f);

            CherryStyle::PushFontSize(0.9f);
            CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.text));
            CherryGUI::TextUnformatted(no_installed_version_available.name.c_str());
            CherryGUI::PopStyleColor();
            CherryStyle::PopFontSize();

            CherryStyle::PushFontSize(0.8f);
            CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.sub));
            CherryGUI::TextUnformatted("Version:");
            CherryGUI::PopStyleColor();
            CherryGUI::SameLine();
            CreatePill(no_installed_version_available.version, pal.accent, pal.accentText);
            CherryStyle::PopFontSize();

            CherryGUI::EndChild();
            CherryGUI::PopStyleVar();
            CherryGUI::PopStyleColor(2);
          }

          CherryGUI::Spacing();
          CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.sub));
          CherryGUI::TextWrapped("%s", Cherry::GetLocale("loc.windows.welcome.can_install_version").c_str());
          CherryGUI::PopStyleColor();
        } else {
          CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(pal.danger));
          if (!VortexMaker::GetCurrentContext()->disconnected) {
            CherryGUI::TextWrapped("%s", Cherry::GetLocale("loc.windows.welcome.cannot_find_version").c_str());
          } else {
            CherryGUI::TextWrapped("%s", Cherry::GetLocale("loc.windows.welcome.cannot_find_version_offline").c_str());
          }
          CherryGUI::PopStyleColor();
        }

        CherryGUI::Spacing();
        CherryGUI::Separator();
        CherryGUI::Spacing();

        auto styled_button = [&](const char* label, const char* bg, const char* bg_hover, const char* fg) -> bool {
          CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
          CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 7.0f));
          CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA(bg));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA(bg_hover));
          CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA(bg_hover));
          CherryGUI::PushStyleColor(ImGuiCol_Text, Cherry::HexToRGBA(fg));
          const bool clicked = CherryGUI::Button(label);
          CherryGUI::PopStyleColor(4);
          CherryGUI::PopStyleVar(2);
          return clicked;
        };

        const char* neutral_hover = pal.dark ? "#3A3A3A" : "#D2D2D2";
        const char* accent_hover = pal.dark ? "#C3FF53" : "#4FA800";

        if (styled_button(Cherry::GetLocale("loc.close").c_str(), pal.pillBg, neutral_hover, pal.pillText)) {
          CherryGUI::CloseCurrentPopup();
          no_installed_modal_opened = false;
        }

        if (!VortexMaker::GetCurrentContext()->disconnected) {
          CherryGUI::SameLine();
          const bool can_search = static_cast<bool>(m_SearchVersionCallback);
          if (!can_search)
            CherryGUI::BeginDisabled();
          if (styled_button("Search official versions", pal.pillBg, neutral_hover, pal.text)) {
            m_SearchVersionCallback(no_installed_version);
            CherryGUI::CloseCurrentPopup();
            no_installed_modal_opened = false;
          }
          if (!can_search)
            CherryGUI::EndDisabled();
          if (CherryGUI::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            CherryGUI::SetTooltip("Open the Version Manager filtered on \"%s\"", no_installed_version.c_str());
          }
        }

        if (no_installed_version_available.version != "") {
          CherryGUI::SameLine();
          if (styled_button(Cherry::GetLocale("loc.install_and_open").c_str(), pal.accent, accent_hover, pal.accentText)) {
            std::thread([this]() {
              VortexMaker::OpenVortexInstaller(
                  no_installed_version_available.version,
                  no_installed_version_available.arch,
                  no_installed_version_available.dist,
                  no_installed_version_available.plat);
            }).detach();
          }
        }

        CherryGUI::EndPopup();
      }

      CherryGUI::PopStyleColor(3);
      CherryGUI::PopStyleVar(2);
    }
  }

}  // namespace VortexLauncher
