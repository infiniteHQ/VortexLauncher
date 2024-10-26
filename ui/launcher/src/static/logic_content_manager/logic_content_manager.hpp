#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../main/include/modules/install.h"
#include "../../../../../main/include/modules/delete.h"
#include "../../../../../main/include/modules/load.h"
#include "../../instances/file_browser/file_browser.hpp"

#include "../../utils.hpp"

#ifndef MODULES_PLUGINS_HANDLER_H
#define MODULES_PLUGINS_HANDLER_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher
{
    struct LogicContentManagerChild
    {
        std::string m_Parent;
        std::string m_ChildName;
        std::function<void()> m_Callback;

        LogicContentManagerChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
                                                                                                                                      m_ChildName(child_name),
                                                                                                                                      m_Callback(child)
        {
        }
    };

    class LogicContentManager : public std::enable_shared_from_this<LogicContentManager>
    {
    public:
        LogicContentManager(const std::string &name);

        void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);

        void RenderImportModules();

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<LogicContentManager> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();
        void SearchModulesOnDirectory(const std::string &path);
        bool MyButton(const std::string &name, const std::string &path, const std::string &description, const std::string &size, bool selected, const std::string &logo, ImU32 bgColor, ImU32 borderColor, ImU32 lineColor, float maxTextWidth, float borderRadius);

        std::vector<LogicContentManagerChild> m_Childs;
        std::string m_SelectedChildName;
        std::shared_ptr<FileBrowserAppWindow> m_FileBrowser;
        std::shared_ptr<Cherry::AppWindow> m_AppWindow;

        std::vector<std::shared_ptr<ModuleInterface>> m_FindedModules;
        std::vector<std::shared_ptr<ModuleInterface>> m_SelectedModules;
        std::vector<std::shared_ptr<ModuleInterface>> m_ModulesToSuppr;
        std::vector<std::shared_ptr<ModuleInterface>> m_ModulesToImport;
        std::atomic<bool> m_StillSearching = false;
        bool m_SearchStarted = false;
        std::string m_SearchElapsedTime;

        bool m_ShowModulesDeletionModal = false;
        bool m_ShowModulesImportModal = false;
        bool m_ShowPluginsDeletionModal = false;
        bool m_ShowPluginsImportModal = false;
        std::vector<int> m_SelectedIds;
        std::mutex m_ModulesMutex;
        int m_ToImportDestinationIndex;
        std::vector<std::string> m_ModulesPool;
        std::string m_ToImportDestination;
    };

}

#endif // MODULES_PLUGINS_HANDLER_H