#include "./content_manager.hpp"

static std::vector<std::shared_ptr<TemplateInterface>> to_suppr_templates;

static bool showTemplatesDeletionModal = false;
static bool showTemplatesImportModal = false;

namespace Cherry
{
    ContentManager::ContentManager(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("ressources/imgs/icons/misc/icon_home.png"));

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

        RegisterAvailableVersions();

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        this->AddChild("Manage content", "Installed template(s)", [this]()
                       {
                       TitleTwo("Installed template(s)");

                       /*ImGui::Text("You can add paths here");
                       ImGui::SameLine();
                       Cherry::TextButtonUnderline("here", true, "#3434F7FF");*/

                       static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                       static std::shared_ptr<Cherry::CustomDrowpdownImageButtonSimple> filter_btn = std::make_shared<Cherry::CustomDrowpdownImageButtonSimple>("LogicContentManager.FindModules.Filter", "####filder");
                       filter_btn->SetScale(0.85f);
                       filter_btn->SetInternalMarginX(10.0f);
                       filter_btn->SetLogoSize(15, 15);

                       filter_btn->SetDropDownImage(Application::CookPath("ressources/imgs/icons/misc/icon_down.png"));
                       filter_btn->SetImagePath(Cherry::GetPath("ressources/imgs/icons/misc/icon_filter.png"));

                       static std::shared_ptr<Cherry::ImageTextButtonSimple> find_in_folder = std::make_shared<Cherry::ImageTextButtonSimple>("find_in_folder", "Find in folder");
                       find_in_folder->SetScale(0.85f);
                       find_in_folder->SetInternalMarginX(10.0f);
                       find_in_folder->SetLogoSize(15, 15);
                       find_in_folder->SetBackgroundColorIdle("#3232F7FF");
                       find_in_folder->SetImagePath(Cherry::GetPath("ressources/imgs/icons/misc/icon_collection.png"));

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

        std::string label = "Delete " + std::to_string(selectedIDs.size()) + " module(s)";


    static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
    del_btn->SetScale(0.85f);
    del_btn->SetInternalMarginX(10.0f);
    del_btn->SetLogoSize(15, 15);
    del_btn->SetBackgroundColorIdle("#00000000");
    del_btn->SetImagePath(Cherry::GetPath("ressources/imgs/icons/misc/icon_trash.png"));
    del_btn->SetLabel(label);


        if(del_btn->Render("del_btn"))
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
                       {
                           //
                       });

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

    void ContentManager::RefreshRender(const std::shared_ptr<ContentManager> &instance)
    {
        m_AppWindow->SetRenderCallback([instance]()
                                       {
                                           static float leftPaneWidth = 300.0f;
                                           const float minPaneWidth = 50.0f;
                                           const float splitterWidth = 1.5f;
                                           static int selected;
      std::map<std::string, std::vector<ContentManagerChild>> groupedByParent;
                                       for (const auto &child : instance->m_Childs)
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
                                               if (child.m_ChildName == instance->m_SelectedChildName)
                                               {
                                                   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                                               }
                                               else
                                               {
                                                   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                                               }

                                               if (TextButtonUnderline(child.m_ChildName.c_str()))
                                               {
                                                   instance->m_SelectedChildName = child.m_ChildName;
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

                                           if(!instance->m_SelectedChildName.empty())
                                           {
                                                std::function<void()> pannel_render = instance->GetChild(instance->m_SelectedChildName);
                                                if(pannel_render)
                                                {
                                                    pannel_render();
                                                }
                                           }
                                        
                                           ImGui::EndGroup(); });
    }

}
