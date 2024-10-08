#include "./main_settings.hpp"

namespace Cherry
{
    MainSettings::MainSettings(const std::string &name)
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

        this->AddChild("Installed content", "Installed Vortex versions", [this]()
                       {
                           Cherry::TitleFourColored("Install latest version ", "#75757575");
                           ImGui::SameLine();
                           Cherry::TitleFourColored("See all versions available ", "#75757575");

                           Cherry::TitleFourColored("All installed version ", "#75757575");
                           for (int row = 0; row < m_VortexRegisteredVersions.size(); row++)
                           {
                               if (areStringsSimilar(m_VortexRegisteredVersions[row]->m_VersionName, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                               {
                                   VersionButton(m_VortexRegisteredVersions[row]->m_VersionName, 300, 100, m_VortexRegisteredVersions[row]->m_Version);
                               }
                           } });

        this->AddChild("Installed content", "Installed modules", [this]()
                       {
                          static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    if (ImGui::BeginTable("modules_tables", 6, flags))
                    {
                        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::PopStyleVar(4);

                        // TODO : Instanciables wins for module/template inspection
                        for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_em.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 6; column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                if (column == 0)
                                {
                                    /*std::string deleteButtonID = "Select###" + std::to_string(row) + "-" + std::to_string(column);
                                    if (ImGui::ImageButtonWithText(projectIcon, deleteButtonID.c_str(), ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
                                    {
                                        selected_module = VortexMaker::GetCurrentContext()->IO.sys_em[row];
                                    }*/
                                }
                                else if (column == 1)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_proper_name.c_str());
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_name.c_str());
                                }
                                else if (column == 3)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_version.c_str());
                                }
                                else if (column == 4)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_path.c_str());
                                }
                                else if (column == 5)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[row]->m_type.c_str());
                                }
                            }
                        }
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                        ImGui::EndTable();
                    } });
        this->AddChild("Installed content", "Installed plugins", [this]()
                       {
                           //
                       });
        this->AddChild("Installed content", "Installed templates", [this]()
                       {
                          static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    if (ImGui::BeginTable("templates_table", 6, flags))
                    {
                        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::PopStyleVar(4);


                        for (int row = 0; row < VortexMaker::GetCurrentContext()->IO.sys_templates.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 6; column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                if (column == 0)
                                {
                                    /*std::string deleteButtonID = "Select###" + std::to_string(row) + "-" + std::to_string(column);
                                    if (ImGui::ImageButtonWithText(projectIcon, deleteButtonID.c_str(), ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
                                    {
                                        selected_module = VortexMaker::GetCurrentContext()->IO.sys_em[row];
                                    }*/
                                }
                                else if (column == 1)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_proper_name.c_str());
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_name.c_str());
                                }
                                else if (column == 3)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_version.c_str());
                                }
                                else if (column == 4)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_path.c_str());
                                }
                                else if (column == 5)
                                {
                                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_templates[row]->m_type.c_str());
                                }
                            }
                        }
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                        ImGui::EndTable();
                    } });
        this->AddChild("Installed content", "Installed marketplace elements", [this]()
                       {
                           //
                       });
        this->AddChild("Settings", "Updates", [this]()
                       {
                           //
                       });
        this->AddChild("Settings", "Account", [this]()
                       {
                           //
                       });

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
        m_AppWindow->SetLeftMenubarCallback([]() {});
        m_AppWindow->SetRightMenubarCallback([win]() {});
    }

    void MainSettings::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs.push_back(MainSettingsChild(parent_name, child_name, child));
    }

    void MainSettings::RemoveChild(const std::string &child_name)
    {
    }

    std::function<void()> MainSettings::GetChild(const std::string &child_name)
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

    void MainSettings::RefreshRender(const std::shared_ptr<MainSettings> &instance)
    {
        m_AppWindow->SetRenderCallback([instance]()
                                       {
                                           static float leftPaneWidth = 300.0f;
                                           const float minPaneWidth = 50.0f;
                                           const float splitterWidth = 1.5f;
                                           static int selected;
      std::map<std::string, std::vector<MainSettingsChild>> groupedByParent;
                                       for (const auto &child : instance->m_Childs)
                                       {
                                           groupedByParent[child.m_Parent].push_back(child);
                                       }

                                           ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);


                            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        
                            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

            TitleThree("Settings of Vortex");
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
