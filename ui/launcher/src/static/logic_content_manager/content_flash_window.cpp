#include "./content_flash_window.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <thread>

namespace VortexLauncher {

  namespace fs = std::filesystem;

  namespace {
    const std::string kApi = "https://api.infinite.si/api/garagevortex/";
    const std::string kInfoApi = "http://api.infinite.si:9000/api/garagevortex/";

    const std::vector<FlashKind>& FlashKinds() {
      static const std::vector<FlashKind> kinds = {
        { "mod:", "Modules", "module" },
        { "plu:", "Plugins", "plugin" },
        { "con:", "Contents", "content" },
        { "tem:", "Templates", "template" },
      };
      return kinds;
    }

    std::string DecodeBase64(const std::string& in) {
      static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
      std::string out;
      int val = 0;
      int bits = -8;
      for (unsigned char c : in) {
        if (c == '=') {
          break;
        }
        auto p = chars.find(c);
        if (p == std::string::npos) {
          return "";
        }
        val = (val << 6) + (int)p;
        bits += 6;
        if (bits >= 0) {
          out.push_back((char)((val >> bits) & 0xFF));
          bits -= 8;
        }
      }
      return out;
    }

    std::string CurrentPlatform() {
#if defined(_WIN32)
      return "windows";
#elif defined(__APPLE__)
      return "macos";
#else
      return "linux";
#endif
    }

    std::string CurrentArch() {
#if defined(__aarch64__) || defined(_M_ARM64)
      return "arm64";
#else
      return "x86_64";
#endif
    }

    std::vector<int> ParseVersion(const std::string& v) {
      std::vector<int> parts;
      std::stringstream ss(v);
      std::string item;
      while (std::getline(ss, item, '.')) {
        try {
          parts.push_back(std::stoi(item));
        } catch (...) {
          parts.push_back(0);
        }
      }
      return parts;
    }

    bool VersionGreater(const std::string& a, const std::string& b) {
      auto pa = ParseVersion(a);
      auto pb = ParseVersion(b);
      size_t n = (std::max)(pa.size(), pb.size());
      for (size_t i = 0; i < n; i++) {
        int va = i < pa.size() ? pa[i] : 0;
        int vb = i < pb.size() ? pb[i] : 0;
        if (va != vb) {
          return va > vb;
        }
      }
      return false;
    }

    std::string Lower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
      return s;
    }

    bool RunCapture(const std::string& cmd, std::string& out) {
#ifdef _WIN32
      FILE* pipe = _popen(cmd.c_str(), "r");
#else
      FILE* pipe = popen(cmd.c_str(), "r");
#endif
      if (!pipe) {
        return false;
      }
      char buf[256];
      while (fgets(buf, sizeof(buf), pipe)) {
        out += buf;
      }
#ifdef _WIN32
      return _pclose(pipe) == 0;
#else
      return pclose(pipe) == 0;
#endif
    }

    bool Sha256(const std::string& file, std::string& out) {
      std::string raw;
#ifdef _WIN32
      if (!RunCapture("CertUtil -hashfile \"" + file + "\" SHA256", raw)) {
        return false;
      }
      std::istringstream iss(raw);
      std::string line;
      int index = 0;
      while (std::getline(iss, line)) {
        if (index++ == 1) {
          for (char c : line) {
            if (!std::isspace((unsigned char)c)) {
              out += c;
            }
          }
          break;
        }
      }
#else
      if (!RunCapture("sha256sum \"" + file + "\" 2>/dev/null", raw) || raw.empty()) {
        raw.clear();
        if (!RunCapture("shasum -a 256 \"" + file + "\" 2>/dev/null", raw)) {
          return false;
        }
      }
      std::istringstream iss(raw);
      iss >> out;
#endif
      out = Lower(out);
      return !out.empty();
    }

    bool StageRelease(
        const nlohmann::json& release,
        const fs::path& dest,
        const std::shared_ptr<StageProgress>& progress,
        std::string& error) {
      if (!release.contains("files") || !release["files"].is_array() || release["files"].empty()) {
        error = "This release has no files to install.";
        return false;
      }

      std::string url = release["files"][0].get<std::string>();
      std::string expected;

      if (release.contains("metadata") && release["metadata"].contains("sums") && release["metadata"]["sums"].is_array()) {
        for (const auto& entry : release["metadata"]["sums"]) {
          if (entry.is_object() && entry.contains(url)) {
            expected = entry.at(url).get<std::string>();
            break;
          }
        }
      }
      if (expected.empty()) {
        error = "No sha256 sum for this release, cannot process.";
        return false;
      }

      std::string filename = url.substr(url.find_last_of('/') + 1);
      size_t q = filename.find('?');
      if (q != std::string::npos) {
        filename = filename.substr(0, q);
      }
      if (filename.empty()) {
        filename = "download.tar.gz";
      }

      std::error_code ec;
      fs::path content = dest / "content";
      fs::create_directories(content, ec);
      if (ec) {
        error = "Unable to prepare the staging folder.";
        return false;
      }

      fs::path archive = dest / filename;

      progress->Set(StageProgress::State::Working, "Downloading " + filename + "...");
      std::string cmd = "curl -L -f -sS -o \"" + archive.string() + "\" \"" + url + "\"";
      if (std::system(cmd.c_str()) != 0 || !fs::exists(archive, ec) || fs::file_size(archive, ec) == 0) {
        error = "Download failed (is curl installed?).";
        return false;
      }

      progress->Set(StageProgress::State::Working, "Verifying integrity (sha256)...");
      std::string actual;
      if (!Sha256(archive.string(), actual)) {
        fs::remove(archive, ec);
        error = "Unable to compute the sha256 sum.";
        return false;
      }
      if (actual != Lower(expected)) {
        fs::remove(archive, ec);
        error = "The sha256 sum does not match, the file may be corrupted or modified.";
        return false;
      }

      progress->Set(StageProgress::State::Working, "Extracting...");
      cmd = "tar -xzf \"" + archive.string() + "\" -C \"" + content.string() + "\"";
      int rc = std::system(cmd.c_str());
      fs::remove(archive, ec);
      if (rc != 0) {
        error = "Extraction failed (is tar installed?).";
        return false;
      }

      return true;
    }

    FlashFetchResult Fetch(const FlashKind& kind, const std::string& uuid) {
      FlashFetchResult res;
      auto& ctx = *VortexMaker::GetCurrentContext();

      if (ctx.disconnected) {
        res.error = "No internet connection";
        return res;
      }

      nlohmann::json info;
      try {
        info = nlohmann::json::parse(ctx.net.GET(kInfoApi + "get_" + kind.api_kind + "?uuid=" + uuid));
      } catch (...) {
        res.error = "Unable to fetch the service API";
        return res;
      }

      res.info.uuid = info.value("uuid", uuid);
      res.info.name = info.value("name", "");
      res.info.proper_name = info.value("proper_name", "");
      res.info.description = info.value("description", "");
      res.info.picture_link = info.value("picture_link", "");
      res.info.banner_link = info.value("banner_link", "");

      nlohmann::json releases;
      try {
        releases = nlohmann::json::parse(
            ctx.net.GET(kApi + "get_content_releases_summary?parent_uuid=" + uuid + "&kind=" + kind.api_kind));
      } catch (...) {
        res.error = "Unable to fetch releases";
        return res;
      }

      if (!releases.contains("releases") || !releases["releases"].is_array()) {
        res.error = "No release found";
        return res;
      }

      const std::string platform = CurrentPlatform();
      const std::string arch = CurrentArch();

      for (auto& item : releases["releases"]) {
        if (!item.is_object()) {
          continue;
        }

        std::vector<std::string> tokens;
        std::stringstream ss(item.value("name", ""));
        std::string tok;
        while (std::getline(ss, tok, ':')) {
          tokens.push_back(tok);
        }
        if (tokens.size() != 5 || tokens[0] != "vx") {
          continue;
        }

        bool cross = tokens[1] == "cross";
        if (!cross && (tokens[1] != platform || tokens[2] != arch)) {
          continue;
        }

        FlashRelease rel;
        rel.uuid = item.value("uuid", "");
        rel.platform = tokens[1];
        rel.arch = tokens[2];
        rel.major = tokens[3];
        rel.version = tokens[4];
        rel.cross = cross;
        res.releases.push_back(rel);
      }

      std::sort(res.releases.begin(), res.releases.end(), [](const FlashRelease& a, const FlashRelease& b) {
        return VersionGreater(a.version, b.version);
      });

      if (res.releases.empty()) {
        res.error = "No compatible version found for your platform.";
        return res;
      }

      res.success = true;
      return res;
    }
  }  // namespace

  void StageProgress::Set(State s, const std::string& text, const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);
    status = text;
    if (s == State::Error) {
      error = text;
    }
    if (!path.empty()) {
      dir = path;
    }
    state = s;
  }

  std::string StageProgress::Status() {
    std::lock_guard<std::mutex> lock(mutex);
    return status;
  }

  std::string StageProgress::Error() {
    std::lock_guard<std::mutex> lock(mutex);
    return error;
  }

  std::string StageProgress::Dir() {
    std::lock_guard<std::mutex> lock(mutex);
    return dir;
  }

  ContentFlashWindow::ContentFlashWindow(
      const std::string& name,
      const std::string& mode,
      PoolsFn pools_of,
      StagedFn on_staged)
      : m_PoolsOf(std::move(pools_of)),
        m_OnStaged(std::move(on_staged)),
        m_Mode(mode == "flash" ? "flash" : "prompt") {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));
    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [this]() { m_AppWindow->SetVisibility(false); };
    m_AppWindow->SetInternalPaddingX(8.0f);
    m_AppWindow->SetInternalPaddingY(8.0f);
  }

  std::shared_ptr<ContentFlashWindow>
  ContentFlashWindow::Create(const std::string& name, const std::string& mode, PoolsFn pools_of, StagedFn on_staged) {
    auto instance =
        std::shared_ptr<ContentFlashWindow>(new ContentFlashWindow(name, mode, std::move(pools_of), std::move(on_staged)));
    instance->SetupRenderCallback();
    return instance;
  }

  std::shared_ptr<Cherry::AppWindow>& ContentFlashWindow::GetAppWindow() {
    return m_AppWindow;
  }

  void ContentFlashWindow::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() { self->Render(); });
  }

  bool ContentFlashWindow::TryProcess(const std::string& raw) {
    if (raw.empty() || raw.length() >= 70) {
      return false;
    }

    std::string decoded = DecodeBase64(raw);
    for (const auto& kind : FlashKinds()) {
      if (decoded.rfind(kind.prefix, 0) == 0 && decoded.size() > kind.prefix.size()) {
        m_Kind = kind;
        StartSearch(decoded.substr(kind.prefix.size()));
        return true;
      }
    }
    return false;
  }

  void ContentFlashWindow::StartSearch(const std::string& uuid) {
    uint64_t token = ++m_Token;
    m_Error.clear();
    m_ReleaseIndex = 0;
    m_PoolIndex = 0;
    m_Progress.reset();
    m_State = State::Loading;

    FlashKind kind = m_Kind;
    std::thread([this, token, kind, uuid]() {
      FlashFetchResult res = Fetch(kind, uuid);
      if (m_Token.load() != token) {
        return;
      }
      {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Result = res;
      }
      m_State = res.success ? State::Ready : State::Error;
    }).detach();
  }

  void ContentFlashWindow::StartInstall(const FlashRelease& release, const std::string& pool) {
    std::string parent_uuid;
    {
      std::lock_guard<std::mutex> lock(m_Mutex);
      parent_uuid = m_Result.info.uuid;
    }

    m_Progress = std::make_shared<StageProgress>();
    m_Notified = false;
    m_PoolChosen = pool;

    auto progress = m_Progress;
    FlashKind kind = m_Kind;
    std::string url = kApi + "get_release/" + kind.api_kind + "?parent_uuid=" + parent_uuid + "&uuid=" + release.uuid;

    std::thread([url, progress, parent_uuid, release, kind]() {
      try {
        progress->Set(StageProgress::State::Working, "Fetching release...");

        auto& ctx = *VortexMaker::GetCurrentContext();
        nlohmann::json json = nlohmann::json::parse(ctx.net.GET(url));

        fs::path dest = fs::temp_directory_path() / ("vx_stage_" + parent_uuid + "_" + release.version);
        std::error_code ec;
        fs::remove_all(dest, ec);

        std::string error;
        if (!StageRelease(json, dest, progress, error)) {
          fs::remove_all(dest, ec);
          progress->Set(StageProgress::State::Error, error);
          return;
        }

        try {
          nlohmann::json payload;
          payload["uuid"] = parent_uuid;
          ctx.net.POST(kApi + "download_" + kind.api_kind, payload.dump());
        } catch (...) {
        }

        progress->Set(StageProgress::State::Done, "Downloaded.", (dest / "content").string());
      } catch (...) {
        progress->Set(StageProgress::State::Error, "Unexpected error while downloading.");
      }
    }).detach();
  }

  void ContentFlashWindow::Render() {
    ImGui::BeginChild("##flash_content", ImVec2(0.0f, 0.0f), true);

    if (m_Mode == "flash") {
      if (!m_ClipboardChecked) {
        m_ClipboardChecked = true;
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard) {
          TryProcess(clipboard);
        }
      }
    } else {
      ImGui::TextUnformatted("Paste a flash code");
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
      ImGui::InputTextWithHint("##flashlink_input", "Paste flashlink here...", m_Input, IM_ARRAYSIZE(m_Input));
      ImGui::SameLine();
      if (ImGui::Button("Search", ImVec2(80.0f, 0.0f))) {
        m_Error = TryProcess(m_Input) ? "" : "Invalid flashlink code.";
      }
      if (!m_Error.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.55f, 0.55f, 1.0f), "%s", m_Error.c_str());
      }
    }

    ImGui::Separator();

    switch (m_State.load()) {
      case State::Waiting: {
        ImGui::TextDisabled(m_Mode == "flash" ? "Click on a FlashLink icon first." : "Enter a code above.");
        break;
      }

      case State::Loading: {
        static const char* frames[] = { "|", "/", "-", "\\" };
        ImGui::Text("Searching %s", frames[(int)(ImGui::GetTime() / 0.15f) % 4]);
        break;
      }

      case State::Error: {
        std::string error;
        {
          std::lock_guard<std::mutex> lock(m_Mutex);
          error = m_Result.error;
        }
        ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f), "An error occurred");
        ImGui::TextWrapped("%s", error.c_str());
        break;
      }

      case State::Ready: {
        FlashFetchResult res;
        {
          std::lock_guard<std::mutex> lock(m_Mutex);
          res = m_Result;
        }

        float avail_w = ImGui::GetContentRegionAvail().x;

        if (!res.info.banner_link.empty()) {
          ImGui::Image(Cherry::GetTexture(Cherry::GetHttpPath(res.info.banner_link)), ImVec2(avail_w, 120.0f));
        }

        ImGui::BeginGroup();
        if (!res.info.picture_link.empty()) {
          ImGui::Image(Cherry::GetTexture(Cherry::GetHttpPath(res.info.picture_link)), ImVec2(64.0f, 64.0f));
          ImGui::SameLine();
        }
        ImGui::BeginGroup();
        ImGui::SetWindowFontScale(1.3f);
        ImGui::TextUnformatted(res.info.proper_name.empty() ? "Unknown" : res.info.proper_name.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::TextDisabled("%s  -  %s", res.info.name.c_str(), m_Kind.kind_id.c_str());
        ImGui::EndGroup();
        ImGui::EndGroup();

        ImGui::TextWrapped("%s", res.info.description.c_str());
        ImGui::Separator();

        std::vector<std::string> labels;
        for (const auto& r : res.releases) {
          labels.push_back(
              r.version + "   (Vortex " + r.major +
              (r.cross ? ", cross-platform)" : ", " + r.platform + " / " + r.arch + ")"));
        }
        if (m_ReleaseIndex < 0 || m_ReleaseIndex >= (int)labels.size()) {
          m_ReleaseIndex = 0;
        }

        ImGui::TextUnformatted("Version :");
        if (ImGui::BeginCombo("##release_combo", labels[m_ReleaseIndex].c_str())) {
          for (int i = 0; i < (int)labels.size(); i++) {
            if (ImGui::Selectable(labels[i].c_str(), i == m_ReleaseIndex)) {
              m_ReleaseIndex = i;
            }
          }
          ImGui::EndCombo();
        }

        std::vector<std::string> pools = m_PoolsOf(m_Kind.kind_id);
        if (pools.empty()) {
          ImGui::TextColored(
              ImVec4(1.0f, 0.75f, 0.3f, 1.0f), "No %s pool configured on this system.", m_Kind.kind_id.c_str());
          break;
        }
        if (m_PoolIndex < 0 || m_PoolIndex >= (int)pools.size()) {
          m_PoolIndex = 0;
        }

        ImGui::TextUnformatted("Install in pool :");
        if (ImGui::BeginCombo("##pool_combo", pools[m_PoolIndex].c_str())) {
          for (int i = 0; i < (int)pools.size(); i++) {
            if (ImGui::Selectable(pools[i].c_str(), i == m_PoolIndex)) {
              m_PoolIndex = i;
            }
          }
          ImGui::EndCombo();
        }

        ImGui::Spacing();

        bool working = m_Progress && m_Progress->state.load() == StageProgress::State::Working;
        if (working) {
          ImGui::BeginDisabled();
        }
        if (ImGui::Button("Download and install", ImVec2(avail_w, 0.0f))) {
          StartInstall(res.releases[m_ReleaseIndex], pools[m_PoolIndex]);
        }
        if (working) {
          ImGui::EndDisabled();
        }

        if (m_Progress) {
          switch (m_Progress->state.load()) {
            case StageProgress::State::Error:
              ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f), "%s", m_Progress->Error().c_str());
              break;
            case StageProgress::State::Done:
              if (!m_Notified) {
                m_Notified = true;
                m_OnStaged(m_Kind.kind_id, m_Progress->Dir(), m_PoolChosen);
                Cherry::DeleteAppWindow(m_AppWindow);
              }
              break;
            default: ImGui::TextUnformatted(m_Progress->Status().c_str()); break;
          }
        }
        break;
      }
    }

    ImGui::Separator();
    if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f))) {
      Cherry::DeleteAppWindow(m_AppWindow);
    }

    ImGui::EndChild();
  }
}  // namespace VortexLauncher