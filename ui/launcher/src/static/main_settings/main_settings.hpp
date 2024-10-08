#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../utils.hpp"

#ifndef MAIN_SETTINGS_H
#define MAIN_SETTINGS_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace Cherry
{
    // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.

    struct MainSettingsChild
    {
        std::string m_Parent;
        std::string m_ChildName;
        std::function<void()> m_Callback;

        MainSettingsChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
                                                                                                                                m_ChildName(child_name),
                                                                                                                                m_Callback(child)
        {
        }
    };

    class MainSettings
    {
    public:
        MainSettings(const std::string &name);

        void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);
        void RefreshRender(const std::shared_ptr<MainSettings> &instance);

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
        {
            return m_AppWindow;
        }

        std::vector<MainSettingsChild> m_Childs;

        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
}

#endif // MAIN_SETTINGS_H