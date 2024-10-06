#pragma once

#ifndef UIKIT_V1_CONTENT_DATA_LOGS_VIEWER
#define UIKIT_V1_CONTENT_DATA_LOGS_VIEWER

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace Cherry
{
    class DataLogsViewer
    {
    public:
        DataLogsViewer(const std::string &name, const std::shared_ptr<AppWindow> &parent);
        DataLogsViewer(const std::string &name, const std::string &path);
        DataLogsViewer();

        void RefreshRender(const std::shared_ptr<DataLogsViewer> &instance);

        void LoadFileContent(const std::string &filePath)
        {
            std::ifstream file(filePath);

            if (file.is_open())
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                v_Data = buffer.str();
            }
            else
            {
                v_Data = "Error: Cannot open the file!";
            }
        }

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
        {
            return m_AppWindow;
        }

    private:
        // Ui Components
        std::shared_ptr<SimpleTable> cp_SimpleTable;

        std::shared_ptr<DoubleKeyValString> cp_NameInput;
        std::shared_ptr<DoubleKeyValString> cp_DescriptionInput;

        std::shared_ptr<ImageStringInput> cp_SearchInput;
        std::shared_ptr<TextEditor> cp_TextEditor;
        std::shared_ptr<std::string> v_SearchValue;

        // Properties Values
        std::shared_ptr<std::string> v_Name;
        std::shared_ptr<std::string> v_Description;

        std::string v_Data;
        std::string m_FilePath;

        // Renderer
        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
}

#endif // UIKIT_V1_CONTENT_DATA_LOGS_VIEWER