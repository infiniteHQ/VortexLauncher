#include "./logic_content_manager.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "../../../../../main/include/modules/delete.h"
#include "../../../../../main/include/plugins/delete.h"

namespace VortexLauncher {

  namespace fs = std::filesystem;

  namespace {
    ContentEntry ToEntry(const std::shared_ptr<ModuleInterface>& p) {
      ContentEntry e;
      e.name = p->m_name;
      e.proper_name = p->m_proper_name;
      e.version = p->m_version;
      e.description = p->m_description;
      e.logo = p->m_logo_path;
      e.banner = p->m_banner_path;
      e.path = p->m_path;
      e.supported_versions = p->m_supported_versions;
      return e;
    }

    ContentEntry ToEntry(const std::shared_ptr<PluginInterface>& p) {
      ContentEntry e;
      e.name = p->m_name;
      e.proper_name = p->m_proper_name;
      e.version = p->m_version;
      e.description = p->m_description;
      e.logo = p->m_logo_path;
      e.path = p->m_path;
      e.supported_versions = p->m_supported_versions;
      return e;
    }

    ContentEntry ToEntry(const std::shared_ptr<TemplateInterface>& p) {
      ContentEntry e;
      e.name = p->m_name;
      e.proper_name = p->m_proper_name;
      e.version = p->m_version;
      e.description = p->m_description;
      e.logo = p->m_logo_path;
      e.path = p->m_path;
      return e;
    }

    ContentEntry ToEntry(const std::shared_ptr<ContentInterface>& p) {
      ContentEntry e;
      e.name = p->m_name;
      e.proper_name = p->m_proper_name;
      e.description = p->m_description;
      e.path = p->m_path;
      return e;
    }

    template<typename T>
    std::vector<ContentEntry> ToEntries(const std::vector<std::shared_ptr<T>>& v) {
      std::vector<ContentEntry> out;
      out.reserve(v.size());
      for (const auto& p : v) {
        if (p) {
          out.push_back(ToEntry(p));
        }
      }
      return out;
    }

    bool ProbeManifest(const std::string& dir, const std::string& manifest, ContentEntry& out) {
      std::ifstream f(fs::path(dir) / manifest);
      if (!f) {
        return false;
      }
      auto j = nlohmann::json::parse(f, nullptr, false);
      if (j.is_discarded() || !j.is_object() || !j.contains("name")) {
        return false;
      }
      out.name = j.value("name", "");
      out.proper_name = j.value("proper_name", out.name);
      out.version = j.value("version", "");
      out.path = dir;
      return true;
    }

    void RemoveDir(const std::string& path) {
      if (path.empty()) {
        return;
      }
      std::error_code ec;
      fs::remove_all(path, ec);
    }

    auto& IO() {
      return VortexMaker::GetCurrentContext()->IO;
    }

    // imgui helpers

    ImVec2 Off(const ImVec2& p, float x, float y) {
      return ImVec2(p.x + x, p.y + y);
    }

    std::string Ellipsize(const std::string& s, float max_w) {
      if (ImGui::CalcTextSize(s.c_str()).x <= max_w) {
        return s;
      }
      std::string out = s;
      while (!out.empty() && ImGui::CalcTextSize((out + "...").c_str()).x > max_w) {
        out.pop_back();
      }
      return out + "...";
    }

    void DrawImageCover(
        ImDrawList* dl,
        const std::string& path,
        const ImVec2& min,
        const ImVec2& max,
        float rounding,
        ImDrawFlags flags) {
      ImTextureID tex = Cherry::GetTexture(path);
      if (!tex) {
        dl->AddRectFilled(min, max, IM_COL32(52, 52, 56, 255), rounding, flags);
        return;
      }

      ImVec2 ts = Cherry::GetTextureSize(path);
      ImVec2 uv0(0.0f, 0.0f);
      ImVec2 uv1(1.0f, 1.0f);
      float bw = max.x - min.x;
      float bh = max.y - min.y;

      if (ts.x > 0.0f && ts.y > 0.0f && bh > 0.0f) {
        float ta = ts.x / ts.y;
        float ba = bw / bh;
        if (ta > ba) {
          float k = ba / ta;
          uv0.x = (1.0f - k) * 0.5f;
          uv1.x = 1.0f - uv0.x;
        } else {
          float k = ta / ba;
          uv0.y = (1.0f - k) * 0.5f;
          uv1.y = 1.0f - uv0.y;
        }
      }
      dl->AddImageRounded(tex, min, max, uv0, uv1, IM_COL32_WHITE, rounding, flags);
    }

    void DrawLogo(ImDrawList* dl, const ContentEntry& e, const ImVec2& p, float s) {
      dl->AddRectFilled(p, Off(p, s, s), IM_COL32(24, 24, 26, 255), 6.0f);

      ImTextureID tex = e.logo.empty() ? nullptr : Cherry::GetTexture(Cherry::GetPath(e.logo));
      if (tex) {
        dl->AddImageRounded(tex, Off(p, 3, 3), Off(p, s - 3, s - 3), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 4.0f);
        return;
      }

      std::string initials = e.proper_name.size() >= 2 ? e.proper_name.substr(0, 2) : "??";
      ImVec2 ts = ImGui::CalcTextSize(initials.c_str());
      dl->AddText(Off(p, (s - ts.x) * 0.5f, (s - ts.y) * 0.5f), IM_COL32(200, 200, 205, 255), initials.c_str());
    }

    bool IconButton(const char* id, const std::string& icon_path, float size) {
      ImVec2 p = ImGui::GetCursorScreenPos();
      bool clicked = ImGui::InvisibleButton(id, ImVec2(size, size));
      bool hovered = ImGui::IsItemHovered();
      ImDrawList* dl = ImGui::GetWindowDrawList();

      if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        dl->AddRectFilled(p, Off(p, size, size), IM_COL32(255, 255, 255, 26), 4.0f);
      }
      if (ImTextureID tex = Cherry::GetTexture(icon_path)) {
        dl->AddImage(tex, Off(p, 4, 4), Off(p, size - 4, size - 4));
      }
      return clicked;
    }

    bool IconTextButton(const char* id, const std::string& icon_path, const char* label, bool enabled = true) {
      const float pad_x = 10.0f;
      const float pad_y = 5.0f;
      const float icon = 14.0f;
      const float gap = 6.0f;

      ImVec2 ts = ImGui::CalcTextSize(label);
      float icon_w = icon_path.empty() ? 0.0f : icon + gap;
      ImVec2 size(pad_x * 2.0f + icon_w + ts.x, ts.y + pad_y * 2.0f);
      ImVec2 p = ImGui::GetCursorScreenPos();

      if (!enabled) {
        ImGui::BeginDisabled();
      }
      bool clicked = ImGui::InvisibleButton(id, size);
      bool hovered = ImGui::IsItemHovered();
      if (!enabled) {
        ImGui::EndDisabled();
      }

      ImDrawList* dl = ImGui::GetWindowDrawList();
      ImU32 bg = !enabled ? IM_COL32(32, 32, 34, 255) : hovered ? IM_COL32(64, 64, 68, 255) : IM_COL32(46, 46, 50, 255);
      ImU32 fg = enabled ? IM_COL32(225, 225, 228, 255) : IM_COL32(110, 110, 114, 255);

      if (hovered && enabled) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      }

      dl->AddRectFilled(p, Off(p, size.x, size.y), bg, 5.0f);
      dl->AddRect(p, Off(p, size.x, size.y), IM_COL32(70, 70, 76, 255), 5.0f);

      float x = p.x + pad_x;
      if (!icon_path.empty()) {
        if (ImTextureID tex = Cherry::GetTexture(icon_path)) {
          ImU32 tint = enabled ? IM_COL32_WHITE : IM_COL32(255, 255, 255, 90);
          dl->AddImage(
              tex,
              ImVec2(x, p.y + (size.y - icon) * 0.5f),
              ImVec2(x + icon, p.y + (size.y + icon) * 0.5f),
              ImVec2(0, 0),
              ImVec2(1, 1),
              tint);
        }
        x += icon + gap;
      }
      dl->AddText(ImVec2(x, p.y + pad_y), fg, label);
      return clicked;
    }

    bool SidebarItem(
        const std::string& id,
        const std::string& icon_path,
        const std::string& label,
        const std::string& count,
        bool selected) {
      const float h = 30.0f;
      float w = ImGui::GetContentRegionAvail().x;
      ImVec2 p = ImGui::GetCursorScreenPos();

      bool clicked = ImGui::InvisibleButton(("##side_" + id).c_str(), ImVec2(w, h));
      bool hovered = ImGui::IsItemHovered();
      ImDrawList* dl = ImGui::GetWindowDrawList();

      if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      }
      if (selected || hovered) {
        dl->AddRectFilled(p, Off(p, w, h), selected ? IM_COL32(69, 69, 72, 255) : IM_COL32(50, 50, 54, 255), 5.0f);
      }

      float x = p.x + 8.0f;
      if (ImTextureID tex = Cherry::GetTexture(icon_path)) {
        dl->AddImage(tex, ImVec2(x, p.y + 7.0f), ImVec2(x + 16.0f, p.y + 23.0f));
      }
      x += 26.0f;

      float ty = p.y + (h - ImGui::GetFontSize()) * 0.5f;
      dl->AddText(ImVec2(x, ty), selected ? IM_COL32(255, 255, 255, 255) : IM_COL32(169, 169, 172, 255), label.c_str());

      if (!count.empty()) {
        ImVec2 cs = ImGui::CalcTextSize(count.c_str());
        dl->AddText(ImVec2(p.x + w - 8.0f - cs.x, ty), IM_COL32(110, 110, 114, 255), count.c_str());
      }
      return clicked;
    }
  }  // namespace

  void LogicalContentManager::BuildKinds() {
    ContentKind plugins;
    plugins.id = "Plugins";
    plugins.label = "plugin(s)";
    plugins.icon = Cherry::GetPath("resources/imgs/plug.png");
    plugins.finder_icon = Cherry::GetPath("resources/imgs/vplug.png");
    plugins.pools = []() -> std::vector<std::string>& { return IO().sys_plugins_pools; };
    plugins.list = []() { return ToEntries(IO().sys_ep); };
    plugins.probe = [](const std::string& path, ContentEntry& out) { return ProbeManifest(path, "plugin.json", out); };
    plugins.install = [](const std::string& path, const std::string& pool) {
      VortexMaker::InstallPluginToSystem(path, pool);
    };
    plugins.remove = [](const std::string& n, const std::string& v) { VortexMaker::DeleteSystemPlugin(n, v); };
    plugins.reload = []() { VortexMaker::LoadSystemPlugins(IO().sys_ep); };
    plugins.check = VortexMaker::CheckPluginInDirectory;

    ContentKind modules;
    modules.id = "Modules";
    modules.label = "module(s)";
    modules.icon = Cherry::GetPath("resources/imgs/box.png");
    modules.finder_icon = Cherry::GetPath("resources/imgs/vbox.png");
    modules.banner_card = true;
    modules.pools = []() -> std::vector<std::string>& { return IO().sys_modules_pools; };
    modules.list = []() { return ToEntries(IO().sys_em); };
    modules.probe = [](const std::string& path, ContentEntry& out) { return ProbeManifest(path, "module.json", out); };
    modules.install = [](const std::string& path, const std::string& pool) {
      VortexMaker::InstallModuleToSystem(path, pool);
    };
    modules.remove = [](const std::string& n, const std::string& v) { VortexMaker::DeleteSystemModule(n, v); };
    modules.reload = []() { VortexMaker::LoadSystemModules(IO().sys_em); };
    modules.check = VortexMaker::CheckModuleInDirectory;

    ContentKind templates;
    templates.id = "Templates";
    templates.label = "template(s)";
    templates.icon = Cherry::GetPath("resources/imgs/box.png");
    templates.finder_icon = Cherry::GetPath("resources/imgs/vbox.png");
    templates.banner_card = true;
    templates.pools = []() -> std::vector<std::string>& { return IO().sys_templates_pools; };
    templates.list = []() { return ToEntries(IO().sys_templates); };
    templates.probe = [](const std::string& path, ContentEntry& out) { return ProbeManifest(path, "template.json", out); };
    templates.install = [](const std::string& path, const std::string& pool) {
      VortexMaker::InstallTemplateOnSystem(path, pool);
    };
    templates.check = VortexMaker::CheckTemplateInDirectory;

    ContentKind contents;
    contents.id = "Contents";
    contents.label = "content(s)";
    contents.icon = Cherry::GetPath("resources/imgs/box.png");
    contents.finder_icon = Cherry::GetPath("resources/imgs/vbox.png");
    contents.banner_card = true;
    contents.pools = []() -> std::vector<std::string>& { return IO().sys_contents_pools; };
    contents.list = []() { return ToEntries(IO().sys_contents); };
    contents.probe = [](const std::string& path, ContentEntry& out) { return ProbeManifest(path, "content.json", out); };
    contents.install = [](const std::string& path, const std::string& pool) {
      VortexMaker::InstallContentOnSystem(path, pool);
    };
    contents.check = VortexMaker::CheckContentInDirectory;

    m_Kinds = { plugins, modules, templates, contents };
    for (const auto& k : m_Kinds) {
      Rebuild(k);
    }
  }

  void LogicalContentManager::Rebuild(const ContentKind& kind) {
    m_States[kind.id].entries = kind.list();
  }

  void LogicalContentManager::OpenImport(const ContentKind& kind) {
    m_AssetFinder = AssetFinder::Create("Import " + kind.label, VortexMaker::getHomeDirectory());

    Cherry::ApplicationSpecification spec;
    spec.Name = "Find " + kind.label;
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
    spec.FavIconPath = kind.finder_icon;
    spec.IconPath = kind.finder_icon;
    spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
    spec.WindowSaves = false;
    spec.MenubarCallback = [this]() {
      if (ImGui::BeginMenu("Window")) {
        if (ImGui::MenuItem("Close")) {
          Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
        }
        ImGui::EndMenu();
      }
    };

    m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);

    if (!kind.pools().empty()) {
      m_AssetFinder->m_TargetPossibilities = kind.pools();
    }

    m_AssetFinder->GetAppWindow()->SetVisibility(true);
    m_AssetFinder->m_ItemToReconize.push_back(
        std::make_shared<AssetFinderItem>(kind.check, kind.id + " sample", kind.id, Cherry::HexToRGBA("#B1FF31")));

    m_ImportKind = kind.id;
    Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
  }

  void LogicalContentManager::SpawnFlashWindow(const std::string& mode) {
    m_FlashCounter++;

    auto win = ContentFlashWindow::Create(
        "flash_link_" + std::to_string(m_FlashCounter),
        mode,
        [this](const std::string& id) {
          for (const auto& k : m_Kinds) {
            if (k.id == id) {
              return k.pools();
            }
          }
          return std::vector<std::string>{};
        },
        [this](const std::string& id, const std::string& dir, const std::string& pool) {
          m_StagedQueue.push_back({ id, dir, pool });
        });

    win->GetAppWindow()->SetVisibility(true);

    Cherry::ApplicationSpecification spec;
    spec.Name = "Use flash link";
    spec.MinHeight = 300;
    spec.MinWidth = 175;
    spec.Height = 700;
    spec.Width = 500;
    spec.DisableLogo = true;
    spec.DisableResize = true;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = win->GetAppWindow()->m_Name;
    spec.UsingCloseCallback = true;
    spec.CloseCallback = [win]() { Cherry::DeleteAppWindow(win->GetAppWindow()); };
    spec.MenubarCallback = []() { };
    spec.FramebarCallback = []() { };
    spec.WindowSaves = false;

    win->GetAppWindow()->AttachOnNewWindow(spec);
    Cherry::AddAppWindow(win->GetAppWindow());
  }

  void LogicalContentManager::HandleFinderResult(const ContentKind& kind) {
    if (!m_AssetFinder || !m_AssetFinder->m_GetFileBrowserPath || m_ImportKind != kind.id) {
      return;
    }
    m_AssetFinder->m_GetFileBrowserPath = false;

    auto& pools = kind.pools();
    size_t index = (size_t)m_AssetFinder->m_TargetPoolIndex;
    std::string pool = index < pools.size() ? pools[index] : "";

    std::vector<InstallSource> sources;
    for (const auto& selected : m_AssetFinder->m_Selected) {
      sources.push_back({ selected, "" });
    }

    m_AssetFinder->GetAppWindow()->SetVisibility(false);
    m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());

    BeginInstall(kind, sources, pool);
  }

  void LogicalContentManager::HandleStaged() {
    if (m_StagedQueue.empty()) {
      return;
    }

    auto queue = std::move(m_StagedQueue);
    m_StagedQueue.clear();

    for (const auto& item : queue) {
      auto it = std::find_if(m_Kinds.begin(), m_Kinds.end(), [&](const ContentKind& k) { return k.id == item.kind_id; });
      if (it == m_Kinds.end()) {
        RemoveDir(fs::path(item.dir).parent_path().string());
        continue;
      }

      std::string root = item.dir;
      ContentEntry probe;
      if (!it->probe(root, probe)) {
        std::vector<fs::path> dirs;
        std::error_code ec;
        for (const auto& d : fs::directory_iterator(root, ec)) {
          if (d.is_directory()) {
            dirs.push_back(d.path());
          }
        }
        if (dirs.size() == 1) {
          root = dirs[0].string();
        }
      }

      m_SelectedChildName = it->id;
      BeginInstall(*it, { { root, fs::path(item.dir).parent_path().string() } }, item.pool);
    }
  }

  void LogicalContentManager::BeginInstall(
      const ContentKind& kind,
      const std::vector<InstallSource>& sources,
      const std::string& pool) {
    auto& st = m_States[kind.id];
    st.pending.clear();
    st.pending_pool = pool;

    for (const auto& src : sources) {
      PendingInstall p;
      p.path = src.path;
      p.cleanup = src.cleanup;

      if (!kind.probe(src.path, p.entry)) {
        kind.install(src.path, pool);
        RemoveDir(src.cleanup);
        continue;
      }

      for (const auto& e : st.entries) {
        if (e.name == p.entry.name) {
          p.existing.push_back(e);
          if (e.version == p.entry.version) {
            p.identical = true;
          }
        }
      }
      p.decision = p.identical ? 1 : 0;
      st.pending.push_back(std::move(p));
    }

    bool has_conflict =
        std::any_of(st.pending.begin(), st.pending.end(), [](const PendingInstall& p) { return !p.existing.empty(); });

    if (has_conflict) {
      st.conflict_request = true;
    } else {
      ApplyInstalls(kind);
    }
  }

  void LogicalContentManager::ApplyInstalls(const ContentKind& kind) {
    auto& st = m_States[kind.id];

    for (const auto& p : st.pending) {
      if (p.decision != 2) {
        if (p.decision == 1 && kind.remove) {
          for (const auto& e : p.existing) {
            kind.remove(e.name, e.version);
          }
        }
        kind.install(p.path, st.pending_pool);
      }
      RemoveDir(p.cleanup);
    }

    st.pending.clear();
    if (kind.reload) {
      kind.reload();
    }
    Rebuild(kind);
  }

  void LogicalContentManager::CancelInstalls(const ContentKind& kind) {
    auto& st = m_States[kind.id];
    for (const auto& p : st.pending) {
      RemoveDir(p.cleanup);
    }
    st.pending.clear();
  }

  void LogicalContentManager::RenderConflictModal(const ContentKind& kind) {
    auto& st = m_States[kind.id];
    const std::string popup = "Already installed###conflict_" + kind.id;

    if (st.conflict_request) {
      ImGui::OpenPopup(popup.c_str());
      st.conflict_request = false;
    }

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
      return;
    }

    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 480.0f);
    ImGui::TextUnformatted(
        "Some content you are installing already exists on your system. Different versions can live side by side, "
        "but identical versions must be replaced.");
    ImGui::PopTextWrapPos();
    ImGui::Separator();

    for (size_t i = 0; i < st.pending.size(); ++i) {
      auto& p = st.pending[i];
      if (p.existing.empty()) {
        continue;
      }

      ImGui::PushID((int)i);
      ImGui::Spacing();
      ImGui::Text("%s  v%s", p.entry.proper_name.c_str(), p.entry.version.c_str());

      std::string versions = "Installed: ";
      for (size_t j = 0; j < p.existing.size(); ++j) {
        versions += (j ? ", v" : "v") + p.existing[j].version;
      }
      ImGui::TextDisabled("%s", versions.c_str());

      if (!p.identical) {
        ImGui::RadioButton("Keep both", &p.decision, 0);
        ImGui::SameLine();
      }
      ImGui::RadioButton(p.existing.size() > 1 ? "Replace all existing" : "Replace", &p.decision, 1);
      ImGui::SameLine();
      ImGui::RadioButton("Skip", &p.decision, 2);

      ImGui::Spacing();
      ImGui::Separator();
      ImGui::PopID();
    }

    ImGui::Spacing();
    if (ImGui::Button("Cancel", ImVec2(140.0f, 32.0f))) {
      CancelInstalls(kind);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Install", ImVec2(140.0f, 32.0f))) {
      ApplyInstalls(kind);
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  void LogicalContentManager::RenderDeleteModal(const ContentKind& kind) {
    auto& st = m_States[kind.id];
    const std::string popup = "Delete###delete_" + kind.id;

    if (st.delete_request) {
      ImGui::OpenPopup(popup.c_str());
      st.delete_request = false;
    }

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
      return;
    }

    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 480.0f);
    ImGui::TextUnformatted(
        "This will only uninstall it from your system. Projects already using it are not affected; remove it from "
        "the project editor if needed.");
    ImGui::PopTextWrapPos();
    ImGui::Separator();
    ImGui::Text("%s  v%s", st.to_delete.proper_name.c_str(), st.to_delete.version.c_str());
    ImGui::Spacing();

    if (ImGui::Button("Cancel", ImVec2(140.0f, 32.0f))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.62f, 0.10f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.14f, 0.14f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.08f, 0.08f, 1.0f));
    bool confirmed = ImGui::Button("Delete", ImVec2(140.0f, 32.0f));
    ImGui::PopStyleColor(3);

    if (confirmed) {
      if (kind.remove) {
        kind.remove(st.to_delete.name, st.to_delete.version);
      }
      if (kind.reload) {
        kind.reload();
      }
      Rebuild(kind);
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  void LogicalContentManager::DrawCard(const ContentKind& kind, const ContentEntry& e, float w, float h) {
    auto& st = m_States[kind.id];
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 o = ImGui::GetCursorScreenPos();

    const float pad = 12.0f;
    const float rounding = 6.0f;
    const float font = ImGui::GetFontSize();

    std::string key = kind.id + "|" + e.path + "|" + e.version;
    ImGui::PushID(key.c_str());

    dl->AddRectFilled(o, Off(o, w, h), IM_COL32(34, 34, 36, 255), rounding);
    dl->AddRect(o, Off(o, w, h), IM_COL32(58, 58, 62, 255), rounding);

    float text_x = pad;
    float title_y = 0.0f;
    float desc_y = 0.0f;

    if (kind.banner_card) {
      const float banner_h = 76.0f;
      std::string banner = e.banner.empty() ? Cherry::GetPath("resources/imgs/def_project_banner.png") : e.banner;
      DrawImageCover(dl, banner, o, Off(o, w, banner_h), rounding, ImDrawFlags_RoundCornersTop);
      DrawLogo(dl, e, Off(o, pad, banner_h - 20.0f), 40.0f);
      title_y = banner_h + 26.0f;
      desc_y = title_y + 24.0f;
    } else {
      DrawLogo(dl, e, Off(o, pad, pad), 32.0f);
      text_x = pad + 32.0f + 10.0f;
      title_y = pad + 8.0f;
      desc_y = pad + 32.0f + 10.0f;
    }

    std::string ver = e.version.empty() ? "" : "v" + e.version;
    ImVec2 vs = ImGui::CalcTextSize(ver.c_str());
    float name_w = (w - pad) - text_x - vs.x - 8.0f;
    std::string name = Ellipsize(e.proper_name.empty() ? e.name : e.proper_name, name_w);

    dl->AddText(Off(o, text_x, title_y), IM_COL32(245, 245, 247, 255), name.c_str());
    dl->AddText(Off(o, w - pad - vs.x, title_y), IM_COL32(120, 120, 126, 255), ver.c_str());

    ImVec2 desc_min = Off(o, pad, desc_y);
    ImVec2 desc_max = Off(o, w - pad, desc_y + font * 2.0f + 2.0f);
    dl->PushClipRect(desc_min, desc_max, true);
    dl->AddText(
        ImGui::GetFont(),
        font,
        desc_min,
        IM_COL32(155, 155, 160, 255),
        e.description.c_str(),
        nullptr,
        desc_max.x - desc_min.x);
    dl->PopClipRect();

    const float btn = 24.0f;
    const float actions_y = h - btn - 10.0f;

    ImGui::SetCursorScreenPos(Off(o, pad, actions_y));
    ImVec2 info_pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("info", ImVec2(btn, btn));
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("%d supported Vortex version(s)", (int)e.supported_versions.size());
      for (const auto& v : e.supported_versions) {
        ImGui::TextDisabled("%s", v.c_str());
      }
      ImGui::EndTooltip();
    }
    ImVec2 qs = ImGui::CalcTextSize("(?)");
    dl->AddText(Off(info_pos, (btn - qs.x) * 0.5f, (btn - qs.y) * 0.5f), IM_COL32(130, 130, 136, 255), "(?)");

    ImGui::SetCursorScreenPos(Off(o, w - pad - btn, actions_y));
    if (IconButton("delete", Cherry::GetPath("resources/imgs/trash.png"), btn)) {
      st.to_delete = e;
      st.delete_request = true;
    }

    ImGui::SetCursorScreenPos(Off(o, w - pad - btn * 2.0f - 6.0f, actions_y));
    if (IconButton("open", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"), btn)) {
      VortexMaker::OpenFolderInFileManager(e.path);
    }

    ImGui::SetCursorScreenPos(o);
    ImGui::Dummy(ImVec2(w, h + 14.0f));
    ImGui::PopID();
  }

  void LogicalContentManager::RenderKind(const ContentKind& kind) {
    auto& st = m_States[kind.id];
    bool offline = VortexMaker::GetCurrentContext()->disconnected;

    Cherry::PushFont("ClashBold");
    ImGui::TextColored(ImVec4(0.47f, 0.47f, 0.47f, 1.0f), "All %s in the system", kind.label.c_str());
    Cherry::PopFont();
    ImGui::Spacing();

    if (IconTextButton("##import", Cherry::GetPath("resources/base/add.png"), "Import")) {
      OpenImport(kind);
    }
    ImGui::SameLine(0.0f, 8.0f);

    if (IconTextButton("##code", Cherry::GetPath("resources/imgs/icons/garage1.png"), "Enter code", !offline)) {
      SpawnFlashWindow("prompt");
    }
    ImGui::SameLine(0.0f, 8.0f);

    if (IconTextButton("##paste", Cherry::GetPath("resources/imgs/icons/misc/icon_lightning.png"), "Paste", !offline)) {
      SpawnFlashWindow("flash");
    }
    ImGui::SameLine(0.0f, 8.0f);

    if (IconTextButton("##browse", Cherry::GetPath("resources/imgs/icons/misc/icon_net.png"), "Browse")) {
      m_WipNotification = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    HandleFinderResult(kind);
    RenderConflictModal(kind);
    RenderDeleteModal(kind);

    if (st.entries.empty()) {
      ImGui::TextDisabled("Nothing installed yet.");
      return;
    }

    const float card_w = 270.0f;
    const float card_h = kind.banner_card ? 190.0f : 124.0f;
    const float gap = 14.0f;

    float avail = ImGui::GetContentRegionAvail().x;
    int cols = (std::max)(1, (int)((avail + gap) / (card_w + gap)));

    int i = 0;
    for (const auto& e : st.entries) {
      if (i % cols != 0) {
        ImGui::SameLine(0.0f, gap);
      }
      DrawCard(kind, e, card_w, card_h);
      i++;
    }
  }

  LogicalContentManager::LogicalContentManager(const std::string& name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png"));
    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };
    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    m_SelectedChildName = "Plugins";
    m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);

    this->AddChild(
        "Help",
        LogicalContentManagerChild(
            []() {
              Cherry::PushFont("ClashBold");
              ImGui::TextColored(ImVec4(0.74f, 0.74f, 0.74f, 1.0f), "Understanding the Vortex approach.");
              Cherry::PopFont();
              ImGui::Spacing();
              ImGui::Separator();
              ImGui::Spacing();

              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
              ImGui::PushTextWrapPos(0.0f);
              ImGui::TextUnformatted(
                  "Logical content consists of modular components that add features to a project and the Vortex Editor. "
                  "The Vortex Editor alone simply provides the project context and enables all logical and static "
                  "content to communicate with each other. The editor guarantees interoperability between components. "
                  "It allows all parts to interact and provides essential utilities like content managers, project "
                  "settings, and the content browser—along with logging and debugging tools.");
              ImGui::PopTextWrapPos();
              ImGui::PopStyleColor();
              ImGui::Spacing();

              if (IconTextButton(
                      "##docs", Cherry::GetPath("resources/imgs/icons/launcher/docs.png"), "Learn and Documentation")) {
                VortexMaker::OpenURL("https://vortex.infinite.si/learn");
              }
            },
            Cherry::GetPath("resources/imgs/help.png")));

    BuildKinds();
    for (const auto& kind : m_Kinds) {
      this->AddChild(kind.id, LogicalContentManagerChild([this, kind]() { RenderKind(kind); }, kind.icon));
    }
  }

  std::vector<std::shared_ptr<EnvProject>> LogicalContentManager::GetMostRecentProjects(
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

  void LogicalContentManager::AddChild(const std::string& child_name, const LogicalContentManagerChild& child) {
    m_Childs[child_name] = child;
  }

  void LogicalContentManager::RemoveChild(const std::string& child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow>& LogicalContentManager::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<LogicalContentManager> LogicalContentManager::Create(const std::string& name) {
    auto instance = std::shared_ptr<LogicalContentManager>(new LogicalContentManager(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void LogicalContentManager::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  LogicalContentManagerChild* LogicalContentManager::GetChild(const std::string& child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void LogicalContentManager::Render() {
    HandleStaged();

    CherryKit::NotificationButton(
        &m_WipNotification,
        4,
        "info",
        "Work in progress",
        "This feature is not available yet. Thanks for your patience.",
        []() { });

    std::string label = "left_pane" + m_AppWindow->m_Name;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
    ImGui::PopStyleVar(2);

    for (const auto& child : m_Childs) {
      const std::string& name = child.first;
      auto state = m_States.find(name);
      std::string count = state != m_States.end() ? "(" + std::to_string(state->second.entries.size()) + ")" : "";

      if (SidebarItem(name, child.second.LogoPath, name, count, name == m_SelectedChildName)) {
        m_SelectedChildName = name;
      }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0.0f, 0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    bool open = ImGui::BeginChild(
        "ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding);
    ImGui::PopStyleVar();

    if (open && !m_SelectedChildName.empty()) {
      if (auto child = GetChild(m_SelectedChildName); child && child->RenderCallback) {
        child->RenderCallback();
      }
    }
    ImGui::EndChild();
  }
}  // namespace VortexLauncher