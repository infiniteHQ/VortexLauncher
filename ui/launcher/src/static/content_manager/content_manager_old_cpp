#include "./content_manager.hpp"

static std::vector<std::shared_ptr<TemplateInterface>> to_suppr_templates;

static bool showTemplatesDeletionModal = false;
static bool showTemplatesImportModal = false;

namespace VortexLauncher
{
    ContentManager::ContentManager(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

        m_AppWindow->SetVisibility(false);
        m_AppWindow->SetCloseCallback([this]()
                                      { m_AppWindow->SetVisibility(false); });

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

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        this->AddChild("Manage content", "Installed template(s)", [this]()
                       {
                       CherryKit::TitleTwo("Installed template(s)");

                       /*ImGui::Text("You can add paths here");
                       ImGui::SameLine();
                       Cherry::TextButtonUnderline("here", true, "#3434F7FF");*/

                       static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                       /*static std::shared_ptr<Cherry::CustomDrowpdownImageButtonSimple> filter_btn = std::make_shared<Cherry::CustomDrowpdownImageButtonSimple>("LogicContentManager.FindModules.Filter", "####filder");
                       filter_btn->SetScale(0.85f);
                       filter_btn->SetInternalMarginX(10.0f);
                       filter_btn->SetLogoSize(15, 15);

                       filter_btn->SetDropDownImage(Cherry::GetPath("resources/imgs/icons/misc/icon_down.png"));
                       filter_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"));*/

                       /*static std::shared_ptr<Cherry::ImageTextButtonSimple> find_in_folder = std::make_shared<Cherry::ImageTextButtonSimple>("find_in_folder", "Find in folder");
                       find_in_folder->SetScale(0.85f);
                       find_in_folder->SetInternalMarginX(10.0f);
                       find_in_folder->SetLogoSize(15, 15);
                       find_in_folder->SetBackgroundColorIdle("#3232F7FF");
                       find_in_folder->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));*/

                       ImGui::Separator();

            CherryKit::ButtonImageDropdown("resources/imgs/icons/misc/icon_filter.png", [&](){
                           if (ImGui::MenuItem("Option 1"))
                           {
                           }
                           if (ImGui::MenuItem("Option 2"))
                           {
                           }
            });


                       ImGui::SameLine();
                       static char ContentPath[256] = "";

                       ImGui::InputText("", ContentPath, sizeof(ContentPath));

                       ImGui::Separator();

                       static std::vector<bool> selectedRows(VortexMaker::GetCurrentContext()->IO.sys_templates.size(), false);

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

                           for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_templates.size(); row++)
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
                                       ImGui::Image(Cherry::GetTexture(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_logo_path), ImVec2(30, 30));
                                   }
                                   else if (column == 3)
                                   {
                                       ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_name.c_str());
                                   }
                                   else if (column == 4)
                                   {
                                       ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_version.c_str());
                                   }
                                   else if (column == 5)
                                   {
                                       ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_path.c_str());
                                   }
                                   else if (column == 6)
                                   {
                                       ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_type.c_str());
                                   }
                               }

                               // Menu contextuel pour chaque ligne
                               if (ImGui::BeginPopup(("context_menu_" + std::to_string(row)).c_str()))
                               {
                                   if (ImGui::MenuItem("Open"))
                                   {
                                       // Action pour "Open"
                                       // Ajoute ta logique ici
                                   }
                                   if (ImGui::MenuItem("Delete"))
                                   {
                                       // Action pour "Delete"
                                       // Ajoute ta logique ici
                                   }
                                   ImGui::EndPopup();
                               }
                           }

                           ImGui::EndTable();
                       }

                       // Collecter les lignes sélectionnées
                       std::vector<int> selectedIDs;
                       for (int i = 0; i < selectedRows.size(); i++)
                       {
                           if (selectedRows[i])
                           {
                               selectedIDs.push_back(i);
                           }
                       }

                       if (selectedIDs.size() > 0)
                       {

        /*std::string label = "Delete " + std::to_string(selectedIDs.size()) + " module(s)";


    static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
    del_btn->SetScale(0.85f);
    del_btn->SetInternalMarginX(10.0f);
    del_btn->SetLogoSize(15, 15);
    del_btn->SetBackgroundColorIdle("#00000000");
    del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"));
    del_btn->SetLabel(label);*/
        if(CherryKit::ButtonImageText("Delete " + std::to_string(selectedIDs.size()) + " module(s)", Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"))->GetData("isClicked") == "true")
        {
            
                           for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_templates.size(); row++)
                           {
                            for(auto &selected : selectedIDs)
                            {
                                if(row == selected)
                                {
                                    to_suppr_templates.push_back(VortexMaker::GetCurrentContext()->IO.sys_templates[row]);
                                }
                            }
                           }

            showTemplatesDeletionModal = true;
        }
                       }
                       
                       if (showTemplatesDeletionModal)
                       {
    ImGui::OpenPopup("Delete module(s)");


    if (ImGui::BeginPopupModal("Delete module(s)", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
                           {
                               std::string text_label = "Are yout sure to delete theses " + std::to_string(selectedIDs.size()) + " modules ? ";
                               ImGui::TextWrapped(text_label.c_str());                               
                               ImGui::TextWrapped("Important: This action will not delete selected modules from projects you've got, this action will only uninstall project available from your system. To delete modules of your projects, go on the project editor, modules manager and delete modules manually.");
                               ImGui::TextWrapped("Theses modules will be deleted :");


            ImGui::Separator();
if (ImGui::BeginTable("modules_will_be_deleted", 5, flags))
                       {
                           ImGui::TableSetupColumn("Image", ImGuiTableColumnFlags_WidthFixed); // Image + Checkbox
                           ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);      // Selectable + Checkbox
                           ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);      // Image + Checkbox
                           ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                           ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                           ImGui::TableHeadersRow();

                           for (int row = 0; row < to_suppr_templates.size(); row++)
                           {
                               ImGui::TableNextRow();
                               for (int column = 0; column < 5; column++)
                               {
                                   ImGui::TableSetColumnIndex(column);
                                   if (column == 0) // Selectable + Checkbox
                                   {                                       
                                    ImGui::Image(Cherry::GetTexture(to_suppr_templates[row]->m_logo_path), ImVec2(30, 30));
                                   }
                                   else if (column == 1) // Checkbox après Selectable
                                   {                                       
                                    ImGui::Text(to_suppr_templates[row]->m_name.c_str());

                                   }
                                   else if (column == 2) // Colonne Image
                                   {
                                    ImGui::Text(to_suppr_templates[row]->m_proper_name.c_str());
                                   }
                                   else if (column == 3)
                                   {
                                       ImGui::Text(to_suppr_templates[row]->m_version.c_str());
                                   }
                                   else if (column == 4)
                                   {
                                       ImGui::Text(to_suppr_templates[row]->m_path.c_str());
                                   }
                               }
                           }

                           ImGui::EndTable();
                       }




            ImGui::Separator();

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                showTemplatesDeletionModal = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Confirm Delete")) {
  for(auto &module : to_suppr_templates)
                {
                    VortexMaker::DeleteSystemTemplate(module->m_name, module->m_version);
                }

                VortexMaker::LoadSystemTemplates(VortexMaker::GetCurrentContext()->IO.sys_templates);

to_suppr_templates.clear();              
                ImGui::CloseCurrentPopup();
                showTemplatesDeletionModal = false;
            }

                               ImGui::EndPopup();
                           }
 
  } });
        this->AddChild("Manage content", "Installed contents(s)", [this]() {

        });

        this->AddChild("Manage content", "Import template(s)", [this]()
                       { RenderImportTemplates(); });

        this->AddChild("Manage content", "Import contents(s)", [this]()
                       {
                           //
                       });

        this->AddChild("Settings", "Template(s) pools", [this]()
                       {
                           //
                       });

        this->AddChild("Settings", "Content(s) pools", [this]()
                       {
                           //
                       });

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
        m_AppWindow->SetLeftMenubarCallback([]() {});
        m_AppWindow->SetRightMenubarCallback([win]() {});
    }

    void ContentManager::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs.push_back(ContentManagerChild(parent_name, child_name, child));
    }

    void ContentManager::RemoveChild(const std::string &child_name)
    {
    }

    std::function<void()> ContentManager::GetChild(const std::string &child_name)
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

    std::shared_ptr<Cherry::AppWindow> &ContentManager::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<ContentManager> ContentManager::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<ContentManager>(new ContentManager(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void ContentManager::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void ContentManager::Render()
    {
        if (m_ShowTemplatesImportModal)
        {
            ImGui::OpenPopup("Import template(s)");

            ImVec2 main_window_size = ImGui::GetWindowSize();
            ImVec2 window_pos = ImGui::GetWindowPos();

            ImGui::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 300, window_pos.y + 150));

            ImGui::SetNextWindowSize(ImVec2(600, 0), ImGuiCond_Always);

            if (ImGui::BeginPopupModal("Import template(s)", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
            {
                std::string text_label = "Are you sure to import these templates ? ";
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

                    for (int row = 0; row < m_TemplatesToImport.size(); row++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 5; column++)
                        {
                            ImGui::TableSetColumnIndex(column);
                            if (column == 0)
                            {
                                ImGui::Image(Cherry::GetTexture(m_TemplatesToImport[row]->m_logo_path), ImVec2(30, 30));
                            }
                            else if (column == 1)
                            {
                                ImGui::Text(m_TemplatesToImport[row]->m_name.c_str());
                            }
                            else if (column == 2)
                            {
                                ImGui::Text(m_TemplatesToImport[row]->m_proper_name.c_str());
                            }
                            else if (column == 3)
                            {
                                ImGui::Text(m_TemplatesToImport[row]->m_version.c_str());
                            }
                            else if (column == 4)
                            {
                                ImGui::Text(m_TemplatesToImport[row]->m_path.c_str());
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
                    m_ShowTemplatesImportModal = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Confirm Import"))
                {
                    for (auto &module : m_TemplatesToImport)
                    {
                        VortexMaker::InstallTemplateOnSystem(module->m_path, m_ToImportDestination);
                    }

                    // VortexMaker::LoadSystemModules(VortexMaker::GetCurrentContext()->IO.sys_em);

                    m_TemplatesToImport.clear();
                    ImGui::CloseCurrentPopup();
                    m_ShowTemplatesImportModal = false;
                }
                ImGui::SameLine();
                /*static std::shared_ptr<Cherry::ComboSimple> combo_dest = std::make_shared<Cherry::ComboSimple>("combo", "####Import to", m_TemplatesPool, 0);
                ImGui::SetNextItemWidth(200.0f);
                combo_dest->Render("qd");
                m_ToImportDestination = combo_dest->GetData("selected_string");*/
                m_ToImportDestination = CherryKit::ComboText("", &m_TemplatesPool)->GetData("selectedString");

                ImGui::EndPopup();
            }
        }

        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;
        std::map<std::string, std::vector<ContentManagerChild>> groupedByParent;
        for (const auto &child : m_Childs)
        {
            groupedByParent[child.m_Parent].push_back(child);
        }

        std::string label = "left_pane" + m_AppWindow->m_Name;
        ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

        CherryKit::TitleThree("Manage Modules & Plugins");
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

                if (CherryKit::ButtonText(child.m_ChildName.c_str()))
                {
                    m_SelectedChildName = child.m_ChildName;
                }

                ImGui::PopStyleColor();
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#44444466"));
        ImGui::Button("splitter", ImVec2(splitterWidth, -1));
        ImGui::PopStyleColor();

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

    void ContentManager::SearchTemplatesOnDirectory(const std::string &path)
    {
        m_StillSearching = true;
        m_SearchStarted = true;
        VortexMaker::FindTemplatesInDirectoryRecursively(path, m_FindedTemplates, m_StillSearching, m_SearchElapsedTime);
    }

    void ContentManager::RenderImportTemplates()
    {
        float childWidth = ImGui::GetContentRegionAvail().x;
        float windowWidth = ImGui::GetWindowSize().x;

        if (m_FileBrowser)
        {
            if (m_FileBrowser->m_GetFileBrowserPath)
            {
                strncpy(ContentPath, m_FileBrowser->m_CurrentDirectory.string().c_str(), sizeof(ContentPath) - 1);
                ContentPath[sizeof(ContentPath) - 1] = '\0';
                m_FileBrowser->m_GetFileBrowserPath = false;
                m_FindedTemplates.clear();

                // SearchModulesOnDirectory(ContentPath);

                m_FileBrowser->GetAppWindow()->SetVisibility(false);
                m_FileBrowser->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
            }
        }

        /*static std::shared_ptr<Cherry::ImageTextButtonSimple> add_btn = std::make_shared<Cherry::ImageTextButtonSimple>("add_btn_module", "####add");
        add_btn->SetScale(0.85f);
        add_btn->SetInternalMarginX(10.0f);
        add_btn->SetLogoSize(15, 15);
        add_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));*/

        /*static std::shared_ptr<Cherry::ImageTextButtonSimple> find_in_folder = std::make_shared<Cherry::ImageTextButtonSimple>("find_in_folder", "");
        find_in_folder->SetScale(0.85f);
        find_in_folder->SetInternalMarginX(10.0f);
        find_in_folder->SetLogoSize(15, 15);
        find_in_folder->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"));*/

        float inputTextWidth = 300.0f;
        float buttonWidth = 100.0f;
        float groupWidth = inputTextWidth + ImGui::GetStyle().ItemSpacing.x + buttonWidth;

        float titleWidth = ImGui::CalcTextSize("Import module(s) to system").x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (childWidth - titleWidth - 30.0f) * 0.5f);
        CherryKit::TitleFour("Import module(s) to system");

        ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#272727FF"));

        float offsetX = (childWidth - groupWidth) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

        ImGui::SetNextItemWidth(inputTextWidth);
        ImGui::InputText("", ContentPath, sizeof(ContentPath));
        ImGui::SameLine();

        {
            /*static std::shared_ptr<Cherry::ImageTextButtonSimple> btn = std::make_shared<Cherry::ImageTextButtonSimple>("find_in_folder", "");
            btn->SetScale(0.85f);
            btn->SetInternalMarginX(10.0f);
            btn->SetLogoSize(15, 15);
            btn->SetBackgroundColorIdle("#3232F7FF");
            btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));*/

        if(CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"))->GetData("isClicked") == "true")
            {
                m_FindedTemplates.clear();
                SearchTemplatesOnDirectory(ContentPath);
            }
        }
        ImGui::SameLine();

        {
            /*static std::shared_ptr<Cherry::CustomDrowpdownImageButtonSimple> btn = std::make_shared<Cherry::CustomDrowpdownImageButtonSimple>("LogicContentManager.FindModules.Filter", "####filder");
            btn->SetScale(0.85f);
            btn->SetInternalMarginX(10.0f);
            btn->SetLogoSize(15, 15);
            btn->SetDropDownImage(Application::CookPath("resources/imgs/icons/misc/icon_down.png"));
            btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"));*/


            CherryKit::ButtonImageDropdown("resources/imgs/icons/misc/icon_filter.png", [&](){
                /*if (m_TemplatesPool.size() > 0 && m_FilterVersions.size() > 0)
                {
                    static std::shared_ptr<Cherry::ComboSimple> combo_pltforms = std::make_shared<Cherry::ComboSimple>("combo3", "####Platforms", m_FilterPlatforms, 0);
                    ImGui::SetNextItemWidth(200.0f);
                    combo_pltforms->Render("qd");
                    m_SelectedPlatform = combo_pltforms->GetData("selected_string");

                    static std::shared_ptr<Cherry::ComboSimple> combo_version = std::make_shared<Cherry::ComboSimple>("combo2", "####Versions", m_FilterVersions, 0);
                    ImGui::SetNextItemWidth(200.0f);
                    combo_version->Render("qd");
                    m_SelectedVersion = combo_version->GetData("selected_string");
                }*/
            });

            /*if (btn->Render("LogicContentManager"))
            {
                ImVec2 mousePos = ImGui::GetMousePos();
                ImGui::SetNextWindowPos(mousePos);
                ImGui::OpenPopup("ContextMenu");
            }

            if (ImGui::BeginPopup("ContextMenu"))
            {
                /*if (m_TemplatesPool.size() > 0 && m_FilterVersions.size() > 0)
                {
                    static std::shared_ptr<Cherry::ComboSimple> combo_pltforms = std::make_shared<Cherry::ComboSimple>("combo3", "####Platforms", m_FilterPlatforms, 0);
                    ImGui::SetNextItemWidth(200.0f);
                    combo_pltforms->Render("qd");
                    m_SelectedPlatform = combo_pltforms->GetData("selected_string");

                    static std::shared_ptr<Cherry::ComboSimple> combo_version = std::make_shared<Cherry::ComboSimple>("combo2", "####Versions", m_FilterVersions, 0);
                    ImGui::SetNextItemWidth(200.0f);
                    combo_version->Render("qd");
                    m_SelectedVersion = combo_version->GetData("selected_string");
                }*
                ImGui::EndPopup();
            }*/
        }

        ImGui::SameLine();

        if(CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))->GetData("isClicked") == "true")
        {
            m_FileBrowser = FileBrowserAppWindow::Create("Select a folder", VortexMaker::getHomeDirectory());
            Cherry::ApplicationSpecification spec;

            std::string name = "Select folder";
            spec.Name = name;
            spec.MinHeight = 500;
            spec.MinWidth = 500;
            spec.Height = 500;
            spec.Width = 950;
            spec.CustomTitlebar = true;
            spec.DisableWindowManagerTitleBar = true;
            spec.WindowOnlyClosable = false;
            spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
            spec.UniqueAppWindowName = m_FileBrowser->GetAppWindow()->m_Name;
            spec.UsingCloseCallback = true;
            spec.FavIconPath = Cherry::GetPath("resources/imgs/icon/misc/icon_folder.png");
            spec.IconPath = Cherry::GetPath("resources/imgs/icon/misc/icon_folder.png");

            spec.MenubarCallback = [this]()
            {
                if (ImGui::BeginMenu("Window"))
                {
                    CherryKit::SeparatorText(Cherry::GetLocale("loc.menubar.menu.general"));

                    if (ImGui::MenuItem("Close"))
                    {
                        Cherry::DeleteAppWindow(m_FileBrowser->GetAppWindow());
                    }

                    ImGui::EndMenu();
                }
            };

            spec.CloseCallback = [this]()
            {
                Cherry::DeleteAppWindow(m_FileBrowser->GetAppWindow());
            };

            spec.DisableTitle = true;
            spec.WindowSaves = false;
            m_FileBrowser->GetAppWindow()->AttachOnNewWindow(spec);

            m_FileBrowser->GetAppWindow()->SetVisibility(true);
            Cherry::AddAppWindow(m_FileBrowser->GetAppWindow());
        }

        ImGui::Separator();

        const float padding = 19.0f;
        const float thumbnailSize = 94.0f;

        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
        ImGui::BeginChild("finded_contents", ImVec2(0, 0 - 45.0f), false);
        ImGui::Columns(columnCount, 0, false);

        std::vector<std::shared_ptr<TemplateInterface>> modules = m_FindedTemplates;

        for (auto &itemEntry : modules)
        {
            bool skip = true;
            /*if (m_SelectedVersion == "All versions")
            {
                skip = false;
            }*/

            /*for (auto ver : itemEntry->m_supported_versions)
            {
                if (m_SelectedVersion == ver)
                {
                    skip = false;
                }
            }*/

            skip = false;

            if (skip)
            {
                continue;
            }

            if (areStringsSimilar(itemEntry->m_proper_name, TemplatesSearch, threshold) || isOnlySpacesOrEmpty(TemplatesSearch))
            {
                if (TemplateCard(itemEntry->m_proper_name, itemEntry->m_path, itemEntry->m_name, itemEntry->m_version, itemEntry->m_selected, itemEntry->m_logo_path, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), Cherry::HexToImU32("#B1FF31FF"), 100.0f, 5.0f))
                {
                    itemEntry->m_selected = !itemEntry->m_selected;
                }
            }

            ImGui::NextColumn();
        }
        ImGui::EndColumns();
        ImGui::EndChild();
        ImGui::PopStyleColor();

        float windowHeight = ImGui::GetWindowSize().y;
        float contentHeight = ImGui::GetCursorPosY();
        float bottomPadding = 10.0f;
        float targetPosY = contentHeight - bottomPadding;

        ImGui::SetCursorPosY(targetPosY);

        ImGui::Separator();

        /*std::string label = "Import selected template(s)";

        static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
        del_btn->SetScale(0.85f);
        del_btn->SetInternalMarginX(10.0f);
        del_btn->SetLogoSize(15, 15);
        del_btn->SetBackgroundColorIdle("#00000000");
        del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"));
        del_btn->SetLabel(label);*/

        if(CherryKit::ButtonImageText("Import selected template(s)", Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"))->GetData("isClicked") == "true")
        {
            m_TemplatesToImport.clear();
            for (auto &mod : modules)
            {
                if (mod->m_selected)
                    m_TemplatesToImport.push_back(mod);
            }

            m_ShowTemplatesImportModal = true;
        }

        ImGui::SameLine();
        m_TemplatesPool = VortexMaker::GetCurrentContext()->IO.sys_templates_pools;

        ImGui::SameLine();

        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("###Search", TemplatesSearch, sizeof(TemplatesSearch));

        ImGui::SameLine();

        if (m_StillSearching)
        {
            std::string label = "Searching..." + std::to_string(m_FindedTemplates.size()) + " module(s) founded yet. Elapsed time : " + m_SearchElapsedTime;
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
                std::string label = "" + std::to_string(m_FindedTemplates.size()) + " module(s) founded on " + m_SearchElapsedTime + ".";
                ImGui::TextWrapped(label.c_str());
            }
            else
            {
                ImGui::TextWrapped("Please select a folder to search for module(s)...");
            }
        }

        ImGui::PopStyleColor();
    }

    bool ContentManager::TemplateCard(const std::string &name, const std::string &path, const std::string &description, const std::string &size, bool selected, const std::string &logo, ImU32 bgColor = IM_COL32(100, 100, 100, 255), ImU32 borderColor = IM_COL32(150, 150, 150, 255), ImU32 lineColor = IM_COL32(255, 255, 0, 255), float maxTextWidth = 100.0f, float borderRadius = 5.0f)
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

        float oldfontsize = ImGui::GetFont()->Scale;
        ImFont *oldFont = ImGui::GetFont();

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

        std::string button_id = path + "squareButtonWithText" + name;
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

        ImTextureID logotexture = Application::GetCurrentRenderedWindow()->get_texture(logo);
        drawList->AddImage(logotexture, logoPos, ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

        ImGui::GetFont()->Scale = 0.7f;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(size.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = oldfontsize;
        ImGui::PopFont();

        ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
        ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
        drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

        ImGui::GetFont()->Scale = 0.9f;
        ImGui::PushFont(ImGui::GetFont());

        ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
        ImGui::SetCursorScreenPos(textPos);
        ImGui::PushItemWidth(maxTextWidth);
        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
        DrawHighlightedText(drawList, textPos, truncatedText.c_str(), TemplatesSearch, highlightColor, textColor, highlightTextColor);

        ImGui::PopItemWidth();

        ImGui::GetFont()->Scale = oldfontsize;
        ImGui::PopFont();

        ImVec2 descriptionPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
        ImGui::SetCursorScreenPos(descriptionPos);

        ImGui::GetFont()->Scale = 0.7f;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(description.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = oldfontsize;
        ImGui::PopFont();

        ImVec2 smallRectPos = ImVec2(cursorPos.x + fixedSize.x - versionBoxWidth - padding, cursorPos.y + fixedSize.y - versionBoxHeight - padding);
        drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + versionBoxWidth, smallRectPos.y + versionBoxHeight), IM_COL32(0, 0, 0, 255), borderRadius);

        float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        if (cursorPos.x + fixedSize.x < windowVisibleX2)
            ImGui::SameLine();

        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

        ImGui::GetFont()->Scale = oldfontsize;

        return pressed;
    }
}
