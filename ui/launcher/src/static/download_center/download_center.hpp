#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../utils.hpp"

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

#ifndef DOWNLOAD_CENTER_H
#define DOWNLOAD_CENTER_H

    // This window can be a "subappwindow" of a parent if you use the constructor with parent parameter.
    class DownloadCenter : public std::enable_shared_from_this<DownloadCenter>
    {
    public:
        DownloadCenter(const std::string &name);

        void AddChild(const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);
        
        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<DownloadCenter> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        std::unordered_map<std::string, std::function<void()>> m_Childs;

        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };


#endif // DOWNLOAD_CENTER_H