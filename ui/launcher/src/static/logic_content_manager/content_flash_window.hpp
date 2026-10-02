#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "../../../../../main/include/modules/delete.h"
#include "../../instances/asset_finder/asset_finder.hpp"

namespace VortexLauncher {

  struct FlashKind {
    std::string prefix;
    std::string kind_id;
    std::string api_kind;
  };

  struct FlashInfo {
    std::string uuid, name, proper_name, description, picture_link, banner_link;
  };

  struct FlashRelease {
    std::string uuid, platform, arch, major, version;
    bool cross = false;
  };

  struct FlashFetchResult {
    bool success = false;
    std::string error;
    FlashInfo info;
    std::vector<FlashRelease> releases;
  };

  struct StageProgress {
    enum class State { Idle, Working, Done, Error };
    std::atomic<State> state{ State::Idle };
    std::mutex mutex;
    std::string status, error, dir;

    void Set(State s, const std::string& text, const std::string& path = "");
    std::string Status();
    std::string Error();
    std::string Dir();
  };

  class ContentFlashWindow : public std::enable_shared_from_this<ContentFlashWindow> {
   public:
    using PoolsFn = std::function<std::vector<std::string>(const std::string&)>;
    using StagedFn = std::function<void(const std::string&, const std::string&, const std::string&)>;

    static std::shared_ptr<ContentFlashWindow>
    Create(const std::string& name, const std::string& mode, PoolsFn pools_of, StagedFn on_staged);

    std::shared_ptr<Cherry::AppWindow>& GetAppWindow();

   private:
    ContentFlashWindow(const std::string& name, const std::string& mode, PoolsFn pools_of, StagedFn on_staged);

    enum class State { Waiting, Loading, Ready, Error };

    void SetupRenderCallback();
    void Render();
    bool TryProcess(const std::string& raw);
    void StartSearch(const std::string& uuid);
    void StartInstall(const FlashRelease& release, const std::string& pool);

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    PoolsFn m_PoolsOf;
    StagedFn m_OnStaged;
    std::string m_Mode;
    FlashKind m_Kind;
    std::string m_Error;
    std::string m_PoolChosen;
    char m_Input[128] = "";
    bool m_ClipboardChecked = false;
    bool m_Notified = false;

    std::atomic<State> m_State{ State::Waiting };
    std::atomic<uint64_t> m_Token{ 0 };
    std::mutex m_Mutex;
    FlashFetchResult m_Result;
    int m_ReleaseIndex = 0;
    int m_PoolIndex = 0;
    std::shared_ptr<StageProgress> m_Progress;
  };
}  // namespace VortexLauncher