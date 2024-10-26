#include "./logic_content_manager.hpp"
using namespace Cherry;

namespace VortexLauncher
{
    LogicContentManager::LogicContentManager(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

        m_AppWindow->SetVisibility(false);
        m_AppWindow->SetCloseCallback([this]()
                                      { m_AppWindow->SetVisibility(false); });

        m_ModulesPool = VortexMaker::GetCurrentContext()->IO.sys_modules_pools;

        m_AppWindow->m_TabMenuCallback = []()
        {
            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                ImGui::Text("Main stuff");
                ImGui::PopStyleColor();

                ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                ImGui::Separator();
                ImGui::PopStyleColor();

                if (ImGui::MenuItem("Logs Utility", "Overview of all logs"))
                {
                }

                if (ImGui::MenuItem("Logs2 Utility", "Overview of all logs"))
                {
                }

                ImGui::EndMenu();
            }
        };

        RegisterAvailableVersions();

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        this->AddChild("Manage content", "Installed module(s)", [this]()
                       {
                           TitleTwo("Installed module(s)");
                           ImGui::TextWrapped("There is every modules installed in your system and available to import on yours projects. Theses projects come from modules pool path(s).");

                           /*ImGui::Text("You can add paths here");
                           ImGui::SameLine();
                           Cherry::TextButtonUnderline("here", true, "#3434F7FF");*/

                           static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                           static std::shared_ptr<Cherry::CustomDrowpdownImageButtonSimple> filter_btn = std::make_shared<Cherry::CustomDrowpdownImageButtonSimple>("LogicContentManager.FindModules.Filter", "####filder");
                           filter_btn->SetScale(0.85f);
                           filter_btn->SetInternalMarginX(10.0f);
                           filter_btn->SetLogoSize(15, 15);

                           filter_btn->SetDropDownImage(Application::CookPath("resources/imgs/icons/misc/icon_down.png"));
                           filter_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"));

                           static std::shared_ptr<Cherry::ImageTextButtonSimple> find_in_folder = std::make_shared<Cherry::ImageTextButtonSimple>("find_in_folder", "Find in folder");
                           find_in_folder->SetScale(0.85f);
                           find_in_folder->SetInternalMarginX(10.0f);
                           find_in_folder->SetLogoSize(15, 15);
                           find_in_folder->SetBackgroundColorIdle("#3232F7FF");
                           find_in_folder->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));

                           ImGui::Separator();

                           if (filter_btn->Render("LogicContentManager"))
                           {
                               ImVec2 mousePos = ImGui::GetMousePos();
                               ImGui::SetNextWindowPos(mousePos);
                               ImGui::OpenPopup("ContextMenu");
                           }

                           if (ImGui::BeginPopup("ContextMenu"))
                           {
                               if (ImGui::MenuItem("Option 1"))
                               {
                               }
                               if (ImGui::MenuItem("Option 2"))
                               {
                               }
                               ImGui::EndPopup();
                           }
                           ImGui::SameLine();
                           static char ContentPath[256] = "";

                           ImGui::InputText("", ContentPath, sizeof(ContentPath));

                           ImGui::Separator();

                           static std::vector<bool> selectedRows(VortexMaker::GetCurrentContext()->IO.sys_em.size(), false);

                           if (ImGui::BeginTable("modules_tables", 6, flags))
                           {
                               ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableSetupColumn("Image", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                               ImGui::TableHeadersRow();

                               for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_em.size(); row++)
                               {
                                   ImGui::TableNextRow();

                                   ImVec2 min = ImGui::GetItemRectMin();
                                   ImVec2 max = ImGui::GetItemRectMax();
                                   bool isHovered = ImGui::IsMouseHoveringRect(min, max);

                                   if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && isHovered)
                                   {
                                       ImGui::OpenPopup(("context_menu_" + std::to_string(row)).c_str());
                                   }

                                   if (isHovered)
                                   {
                                       ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImColor(0.3f, 0.3f, 0.3f));
                                   }
                                   if (selectedRows[row])
                                   {
                                       ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImColor(0.5f, 0.5f, 0.9f));
                                   }

                                   for (int column = 0; column < 7; column++)
                                   {
                                       ImGui::TableSetColumnIndex(column);
                                       if (column == 0)
                                       {
                                           ImGui::PushID(row);

                                           if (ImGui::Selectable("##RowSelectable", selectedRows[row], ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 30)))
                                           {
                                               bool isCtrlPressed = ImGui::GetIO().KeyCtrl;

                                               if (!isCtrlPressed)
                                               {
                                                   for (int i = 0; i < selectedRows.size(); ++i)
                                                   {
                                                       selectedRows[i] = false;
                                                   }
                                               }

                                               selectedRows[row] = !selectedRows[row];
                                           }
                                       }
                                       else if (column == 1)
                                       {
                                           bool checkboxValue = selectedRows[row];
                                           if (ImGui::Checkbox("##SelectedCheckboxAfterImage", &checkboxValue))
                                           {
                                               selectedRows[row] = checkboxValue;
                                           }
                                       }
                                       else if (column == 2)
                                       {
                                           ImGui::Image(Cherry::GetTexture(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_logo_path), ImVec2(30, 30));
                                       }
                                       else if (column == 3)
                                       {
                                           ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_name.c_str());
                                       }
                                       else if (column == 4)
                                       {
                                           ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_version.c_str());
                                       }
                                       else if (column == 5)
                                       {
                                           ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_path.c_str());
                                       }
                                       else if (column == 6)
                                       {
                                           ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_type.c_str());
                                       }
                                   }

                                   if (ImGui::BeginPopup(("context_menu_" + std::to_string(row)).c_str()))
                                   {
                                       if (ImGui::MenuItem("Open"))
                                       {
                                       }
                                       if (ImGui::MenuItem("Delete"))
                                       {
                                       }
                                       ImGui::EndPopup();
                                   }
                               }

                               ImGui::EndTable();
                           }

                           m_SelectedIds.clear();
                           for (int i = 0; i < selectedRows.size(); i++)
                           {
                               if (selectedRows[i])
                               {
                                   m_SelectedIds.push_back(i);
                               }
                           }

                           if (m_SelectedIds.size() > 0)
                           {

                               std::string label = "Delete " + std::to_string(m_SelectedIds.size()) + " module(s)";

                               static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                               del_btn->SetScale(0.85f);
                               del_btn->SetInternalMarginX(10.0f);
                               del_btn->SetLogoSize(15, 15);
                               del_btn->SetBackgroundColorIdle("#00000000");
                               del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"));
                               del_btn->SetLabel(label);

                               if (del_btn->Render("del_btn"))
                               {

                                   for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_em.size(); row++)
                                   {
                                       for (auto &selected : m_SelectedIds)
                                       {
                                           if (row == selected)
                                           {
                                               m_ModulesToSuppr.push_back(VortexMaker::GetCurrentContext()->IO.sys_em[row]);
                                           }
                                       }
                                   }

                                   m_ShowModulesDeletionModal = true;
                               }
                           } });

        this->AddChild("Manage content", "Installed plugin(s)", [this]()
                       { TitleTwo("Plugins aren't available for the moment"); });

        this->AddChild("Import content", "Import module(s)", [this]()
                       { RenderImportModules(); });

        this->AddChild("Import content", "Import plugin(s)", [this]()
                       {
                           TitleTwo("Import plugin(s)");
                           ImGui::TextWrapped("This section allow you to import plugin(s) into one of your plugins pools. After that you will can install these plugins into your projects");
                           //
                       });

        this->AddChild("Settings", "Modules pools", [this]()
                       {
                           //
                       });

        this->AddChild("Settings", "Plugins pools", [this]()
                       {
                           //
                       });

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
        m_AppWindow->SetLeftMenubarCallback([]() {});
        m_AppWindow->SetRightMenubarCallback([win]() {});
    }

    void LogicContentManager::SearchModulesOnDirectory(const std::string &path)
    {
        m_StillSearching = true;
        m_SearchStarted = true;
        VortexMaker::StartRecursiveModuleSearch(path, m_FindedModules, m_StillSearching, m_SearchElapsedTime);
    }

    void LogicContentManager::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs.push_back(LogicContentManagerChild(parent_name, child_name, child));
    }

    void LogicContentManager::RemoveChild(const std::string &child_name)
    {
        //
    }

    bool LogicContentManager::MyButton(const std::string &name, const std::string &path, const std::string &description, const std::string &size, bool selected, const std::string &logo, ImU32 bgColor = IM_COL32(100, 100, 100, 255), ImU32 borderColor = IM_COL32(150, 150, 150, 255), ImU32 lineColor = IM_COL32(255, 255, 0, 255), float maxTextWidth = 100.0f, float borderRadius = 5.0f)
    {
        bool pressed = false;

        float logoSize = 60.0f;
        float extraHeight = 80.0f;
        float padding = 10.0f;
        float separatorHeight = 2.0f;
        float textOffsetY = 5.0f;
        float versionBoxWidth = 10.0f;
        float versionBoxHeight = 20.0f;
        float thumbnailIconOffsetY = 30.0f;

        if (selected)
        {
            bgColor = IM_COL32(80, 80, 240, 255);
            borderColor = IM_COL32(150, 150, 255, 255);
        }

        ImVec2 squareSize(logoSize, logoSize);

        const char *originalText = name.c_str();
        std::string truncatedText = name;

        if (ImGui::CalcTextSize(originalText).x > maxTextWidth)
        {
            truncatedText = name.substr(0, 20);
            if (ImGui::CalcTextSize(truncatedText.c_str()).x > maxTextWidth)
            {
                truncatedText = name.substr(0, 10) + "\n" + name.substr(10, 10);
            }
        }
        else
        {
            truncatedText = name + "\n";
        }

        ImVec2 fixedSize(maxTextWidth + padding * 2, logoSize + extraHeight + padding * 2);

        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        std::string button_id = name + "squareButtonWithText" + name;
        if (ImGui::InvisibleButton(button_id.c_str(), fixedSize))
        {
            pressed = true;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
        ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

        ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

        static bool open_deletion_modal = false;

        static bool delete_single_file = false;
        static std::string delete_single_file_path = "";

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        ImDrawList *drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), bgColor, borderRadius);
        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y), IM_COL32(26, 26, 26, 255), borderRadius, ImDrawFlags_RoundCornersTop);
        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), borderColor, borderRadius, 0, 1.0f);

        ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + padding);

        ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 20 + textOffsetY);
        ImGui::SetCursorScreenPos(sizePos);

        static ImTextureID logotexture = Application::GetCurrentRenderedWindow()->get_texture(logo);
        drawList->AddImage(logotexture, logoPos, ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

        ImGui::GetFont()->Scale *= 0.7;
        ImGui::PushFont(ImGui::GetFont());
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(size.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = 1.0f;
        ImGui::PopFont();

        ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
        ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
        drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

        ImGui::GetFont()->Scale *= 0.9;
        ImGui::PushFont(ImGui::GetFont());

        ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
        ImGui::SetCursorScreenPos(textPos);
        ImGui::PushItemWidth(maxTextWidth);
        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

        ImGui::PopItemWidth();

        ImGui::GetFont()->Scale = 1.0f;
        ImGui::PopFont();

        ImVec2 descriptionPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
        ImGui::SetCursorScreenPos(descriptionPos);

        ImGui::GetFont()->Scale *= 0.7;
        ImGui::PushFont(ImGui::GetFont());
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(description.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = 1.0f;
        ImGui::PopFont();

        ImVec2 smallRectPos = ImVec2(cursorPos.x + fixedSize.x - versionBoxWidth - padding, cursorPos.y + fixedSize.y - versionBoxHeight - padding);
        drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + versionBoxWidth, smallRectPos.y + versionBoxHeight), IM_COL32(0, 0, 0, 255), borderRadius);

        float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        if (cursorPos.x + fixedSize.x < windowVisibleX2)
            ImGui::SameLine();

        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

        return pressed;
    }

    void LogicContentManager::RenderImportModules()
    {
        TitleTwo("Import module(s)");
        ImGui::TextWrapped("This section allow you to import module(s) into one of your plugins pools. After that you will can install these plugins into your projects");

        if (m_StillSearching)
        {
            std::string label = "Searching..." + std::to_string(m_FindedModules.size()) + " module(s) founded yet. Elapsed time : " + m_SearchElapsedTime;
            ImGui::TextWrapped(label.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                m_StillSearching = false;
            }
        }
        else
        {
            if (m_SearchStarted)
            {
                std::string label = "" + std::to_string(m_FindedModules.size()) + " module(s) founded on " + m_SearchElapsedTime + ".";
                ImGui::TextWrapped(label.c_str());
            }
            else
            {
                ImGui::TextWrapped("Please select a folder to search for module(s)...");
            }
        }

        static std::vector<bool> selectedRows(m_FindedModules.size(), false);
        static char ContentPath[512] = "";
        if (m_FileBrowser)
        {
            if (m_FileBrowser->m_GetFileBrowserPath)
            {
                strncpy(ContentPath, m_FileBrowser->m_CurrentDirectory.c_str(), sizeof(ContentPath) - 1);
                ContentPath[sizeof(ContentPath) - 1] = '\0';
                m_FileBrowser->m_GetFileBrowserPath = false;
                m_FindedModules.clear();

                SearchModulesOnDirectory(ContentPath);

                m_FileBrowser->GetAppWindow()->SetVisibility(false);
                m_FileBrowser->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
            }
        }

        selectedRows.resize(m_FindedModules.size());

        static std::shared_ptr<Cherry::ImageTextButtonSimple> add_btn = std::make_shared<Cherry::ImageTextButtonSimple>("add_btn_module", "####add");
        add_btn->SetScale(0.85f);
        add_btn->SetInternalMarginX(10.0f);
        add_btn->SetLogoSize(15, 15);
        add_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));

        static std::shared_ptr<Cherry::ImageTextButtonSimple> find_in_folder = std::make_shared<Cherry::ImageTextButtonSimple>("find_in_folder", "Find in folder");
        find_in_folder->SetScale(0.85f);
        find_in_folder->SetInternalMarginX(10.0f);
        find_in_folder->SetLogoSize(15, 15);
        find_in_folder->SetBackgroundColorIdle("#3232F7FF");
        find_in_folder->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));

        ImGui::Separator();

        ImGui::InputText("", ContentPath, sizeof(ContentPath));
        ImGui::SameLine();
        if (find_in_folder->Render("qsdqsd"))
        {
            m_FileBrowser = FileBrowserAppWindow::Create("Select a folder", VortexMaker::getHomeDirectory());
            Cherry::ApplicationSpecification spec;

            std::string name = "Select folder";
            spec.Name = name;
            spec.MinHeight = 500;
            spec.MinWidth = 500;
            spec.Height = 500;
            spec.Width = 950;
            spec.CustomTitlebar = false;
            spec.DisableWindowManagerTitleBar = false;
            spec.WindowOnlyClosable = false;
            spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
            spec.UniqueAppWindowName = m_FileBrowser->GetAppWindow()->m_Name;

            spec.DisableTitle = true;
            spec.WindowSaves = false;
            spec.IconPath = Cherry::GetPath("resources/imgs/icon_update.png");
            m_FileBrowser->GetAppWindow()->AttachOnNewWindow(spec);

            m_FileBrowser->GetAppWindow()->SetVisibility(true);
            Cherry::AddAppWindow(m_FileBrowser->GetAppWindow());
        }

        std::cout << "Modules : " << m_FindedModules.size() << std::endl;
        ;

        const float padding = 10.0f;
        const float thumbnailSize = 94.0f;

        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto &itemEntry : m_FindedModules)
        {

            bool selected = false;

            if (MyButton(itemEntry->m_proper_name, itemEntry->m_path, itemEntry->m_name, itemEntry->m_version, itemEntry->m_selected, Application::CookPath("ressources/imgs/icons/files/icon_picture_file.png"), IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), Cherry::HexToImU32("#B1FF31FF")))
            {
                itemEntry->m_selected = !itemEntry->m_selected;

                            m_SelectedModules.push_back(itemEntry);
                    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
                    {
                        if (itemEntry->m_selected)
                        {
                            m_SelectedModules.push_back(itemEntry);
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        if (itemEntry->m_selected)
                        {
                            m_SelectedModules.clear();
                            m_SelectedModules.push_back(itemEntry);
                        }
                        else
                        {
                            m_SelectedModules.erase(
                                std::remove(m_SelectedModules.begin(), m_SelectedModules.end(), itemEntry),
                                m_SelectedModules.end());
                        }
                    }
                
            }

            ImGui::PopID();
            ImGui::NextColumn();
        }

        if (m_SelectedModules.size() > 0)
        {
            std::string label = "Import " + std::to_string(m_SelectedModules.size()) + " module(s)";

            static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
            del_btn->SetScale(0.85f);
            del_btn->SetInternalMarginX(10.0f);
            del_btn->SetLogoSize(15, 15);
            del_btn->SetBackgroundColorIdle("#00000000");
            del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"));
            del_btn->SetLabel(label);

            if (del_btn->Render("del_btn"))
            {
                for (auto &selected : m_SelectedModules)
                {
                    m_ModulesToImport.push_back(selected);
                }

                m_ShowModulesImportModal = true;
            }

            ImGui::SameLine();

            static std::shared_ptr<Cherry::ComboSimple> combo_dest = std::make_shared<Cherry::ComboSimple>("combo", "Import to", m_ModulesPool, 0);
            combo_dest->Render("qd");
            m_ToImportDestination = combo_dest->GetData("selected_string");
        }
    }

    std::function<void()> LogicContentManager::GetChild(const std::string &child_name)
    {
        for (auto &child : m_Childs)
        {
            if (child.m_ChildName == child_name)
            {
                return child.m_Callback;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Cherry::AppWindow> &LogicContentManager::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<LogicContentManager> LogicContentManager::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<LogicContentManager>(new LogicContentManager(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void LogicContentManager::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void LogicContentManager::Render()
    {
        if (m_ShowModulesImportModal)
        {
            ImGui::OpenPopup("Import module(s)");

            ImVec2 main_window_size = ImGui::GetWindowSize();
            ImVec2 window_pos = ImGui::GetWindowPos();

            ImGui::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 300, window_pos.y + 150));

            ImGui::SetNextWindowSize(ImVec2(600, 0), ImGuiCond_Always);

            if (ImGui::BeginPopupModal("Import module(s)", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
            {
                std::string text_label = "Are you sure to import these " + std::to_string(m_SelectedIds.size()) + " modules ? ";
                ImGui::TextWrapped(text_label.c_str());
                ImGui::TextWrapped("Important: This action will not delete/change selected modules...");
                ImGui::TextWrapped("These modules will be imported:");

                ImGui::Separator();

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                float max_table_height = 200.0f;
                ImGui::BeginChild("TableScrollRegion", ImVec2(0, max_table_height), true, ImGuiWindowFlags_HorizontalScrollbar);

                if (ImGui::BeginTable("modules_will_be_imported", 5, flags))
                {
                    ImGui::TableSetupColumn("Image", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    for (int row = 0; row < m_ModulesToImport.size(); row++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 5; column++)
                        {
                            ImGui::TableSetColumnIndex(column);
                            if (column == 0) // Selectable + Checkbox
                            {
                                ImGui::Image(Cherry::GetTexture(m_ModulesToImport[row]->m_logo_path), ImVec2(30, 30));
                            }
                            else if (column == 1) // Checkbox après Selectable
                            {
                                ImGui::Text(m_ModulesToImport[row]->m_name.c_str());
                            }
                            else if (column == 2) // Colonne Image
                            {
                                ImGui::Text(m_ModulesToImport[row]->m_proper_name.c_str());
                            }
                            else if (column == 3)
                            {
                                ImGui::Text(m_ModulesToImport[row]->m_version.c_str());
                            }
                            else if (column == 4)
                            {
                                ImGui::Text(m_ModulesToImport[row]->m_path.c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }

                ImGui::EndChild();

                ImGui::Separator();

                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                    m_ShowModulesImportModal = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Confirm Import"))
                {
                    for (auto &module : m_ModulesToImport)
                    {
                        VortexMaker::InstallModuleToSystem(module->m_path, m_ToImportDestination);
                    }

                    VortexMaker::LoadSystemModules(VortexMaker::GetCurrentContext()->IO.sys_em);

                    m_ModulesToImport.clear();
                    ImGui::CloseCurrentPopup();
                    m_ShowModulesImportModal = false;
                }

                ImGui::EndPopup();
            }
        }

        if (m_ShowModulesDeletionModal)
        {
            ImGui::OpenPopup("Delete module(s)");

            ImVec2 main_window_size = ImGui::GetWindowSize();
            ImVec2 window_pos = ImGui::GetWindowPos();

            ImGui::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 300, window_pos.y + 150));

            ImGui::SetNextWindowSize(ImVec2(600, 0), ImGuiCond_Always);

            if (ImGui::BeginPopupModal("Delete module(s)", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
            {
                std::string text_label = "Are yout sure to delete theses " + std::to_string(m_SelectedIds.size()) + " modules ? ";
                ImGui::TextWrapped(text_label.c_str());
                ImGui::TextWrapped("Important: This action will not delete selected modules from projects you've got, this action will only uninstall project available from your system. To delete modules of your projects, go on the project editor, modules manager and delete modules manually.");
                ImGui::TextWrapped("Theses modules will be deleted :");

                ImGui::Separator();

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("modules_will_be_deleted", 5, flags))
                {
                    ImGui::TableSetupColumn("Image", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    for (int row = 0; row < m_ModulesToSuppr.size(); row++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 5; column++)
                        {
                            ImGui::TableSetColumnIndex(column);
                            if (column == 0)
                            {
                                ImGui::Image(Cherry::GetTexture(m_ModulesToSuppr[row]->m_logo_path), ImVec2(30, 30));
                            }
                            else if (column == 1)
                            {
                                ImGui::Text(m_ModulesToSuppr[row]->m_name.c_str());
                            }
                            else if (column == 2)
                            {
                                ImGui::Text(m_ModulesToSuppr[row]->m_proper_name.c_str());
                            }
                            else if (column == 3)
                            {
                                ImGui::Text(m_ModulesToSuppr[row]->m_version.c_str());
                            }
                            else if (column == 4)
                            {
                                ImGui::Text(m_ModulesToSuppr[row]->m_path.c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }

                ImGui::Separator();

                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                    m_ShowModulesDeletionModal = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Confirm Delete"))
                {
                    for (auto &module : m_ModulesToSuppr)
                    {
                        VortexMaker::DeleteSystemModule(module->m_name, module->m_version);
                    }

                    VortexMaker::LoadSystemModules(VortexMaker::GetCurrentContext()->IO.sys_em);

                    m_ModulesToSuppr.clear();
                    ImGui::CloseCurrentPopup();
                    m_ShowModulesDeletionModal = false;
                }

                ImGui::EndPopup();
            }
        }

        static float leftPaneWidth = 300.0f;
        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;
        static int selected;
        std::map<std::string, std::vector<LogicContentManagerChild>> groupedByParent;
        for (const auto &child : m_Childs)
        {
            groupedByParent[child.m_Parent].push_back(child);
        }

        ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

        TitleThree("Manage Modules & Plugins");
        for (const auto &[parent, children] : groupedByParent)
        {

            ImGui::GetFont()->Scale *= 0.8;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
            ImGui::Text(parent.c_str());
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
            ImGui::Separator();
            ImGui::PopStyleColor();

            ImGui::GetFont()->Scale = 0.84;
            ImGui::PopFont();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

            for (const auto &child : children)
            {
                if (child.m_ChildName == m_SelectedChildName)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                }

                if (TextButtonUnderline(child.m_ChildName.c_str()))
                {
                    m_SelectedChildName = child.m_ChildName;
                }

                ImGui::PopStyleColor();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, HexToRGBA("#44444466"));
        ImGui::Button("splitter", ImVec2(splitterWidth, -1));
        ImGui::PopStyleVar();

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (ImGui::IsItemActive())
        {
            float delta = ImGui::GetIO().MouseDelta.x;
            leftPaneWidth += delta;
            if (leftPaneWidth < minPaneWidth)
                leftPaneWidth = minPaneWidth;
        }

        ImGui::SameLine();
        ImGui::BeginGroup();

        if (!m_SelectedChildName.empty())
        {
            std::function<void()> pannel_render = GetChild(m_SelectedChildName);
            if (pannel_render)
            {
                pannel_render();
            }
        }

        ImGui::EndGroup();
    }
} // namespace VortexLauncher