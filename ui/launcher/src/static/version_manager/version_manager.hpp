#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../main/include/modules/install.h"
#include "../../../../../main/include/modules/delete.h"
#include "../../../../../main/include/modules/load.h"
#include "../../instances/file_browser/file_browser.hpp"

#include "../../utils.hpp"

#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

struct VersionManagerAppWindowChild
{
    std::string m_Parent;
    std::string m_ChildName;
    std::function<void()> m_Callback;

    VersionManagerAppWindowChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
                                                                                                                                      m_ChildName(child_name),
                                                                                                                                      m_Callback(child)
    {
    }
};

class VersionManagerAppWindow : public std::enable_shared_from_this<VersionManagerAppWindow>
{
public:
    VersionManagerAppWindow(const std::string &name);

    void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
    void RemoveChild(const std::string &child_name);
    std::function<void()> GetChild(const std::string &child_name);
    void RefreshRender(const std::shared_ptr<VersionManagerAppWindow> &instance);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<VersionManagerAppWindow> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();
    std::vector<VersionManagerAppWindowChild> m_Childs;

    std::string m_SelectedChildName;
    std::shared_ptr<FileBrowserAppWindow> m_FileBrowser;

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
     float leftPaneWidth = 300.0f;
     int selected;
};

#endif // VERSION_MANAGER_H