#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../main/include/templates/install.h"
#include "../../../../../main/include/templates/delete.h"
#include "../../../../../main/include/templates/load.h"
#include "../../utils.hpp"

#ifndef CONTENT_MANAGER_H
#define CONTENT_MANAGER_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexLauncher
{
    // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

    struct ContentManagerChild
    {
        std::string m_Parent;
        std::string m_ChildName;
        std::function<void()> m_Callback;

        ContentManagerChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
                                                                                                                                m_ChildName(child_name),
                                                                                                                                m_Callback(child)
        {
        }
    };

    class ContentManager : public std::enable_shared_from_this<ContentManager>
    {
    public:
        ContentManager(const std::string &name);

        void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);
        void RefreshRender(const std::shared_ptr<ContentManager> &instance);


        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<ContentManager> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        std::vector<ContentManagerChild> m_Childs;

        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
        float leftPaneWidth = 300.0f;
        int selected;
    };
}

#endif // CONTENT_MANAGER_H