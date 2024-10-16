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
        
        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<LogicContentManager> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        std::vector<LogicContentManagerChild> m_Childs;

        std::string m_SelectedChildName;
        std::shared_ptr<FileBrowserAppWindow> m_FileBrowser;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };

}

#endif // MODULES_PLUGINS_HANDLER_H