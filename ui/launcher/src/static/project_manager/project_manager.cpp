
#include "project_manager.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_set>
static std::vector<std::string> projectPoolsPaths = {};
static std::vector<bool> selectedRows(finded_projects.size(), false);

std::vector<std::string> modules_projects;
static std::vector<int> selectedIDs;

int projetct_import_dest_index;
std::vector<std::string> project_pools;
std::string projetct_import_dest;

ProjectManager::ProjectManager(const std::string &name)
{
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));
    m_AppWindow->SetDefaultBehavior(Cherry::DefaultAppWindowBehaviors::DefaultDocking, "full");
    m_AppWindow->SetClosable(false);

    project_pools = VortexMaker::GetCurrentContext()->IO.sys_projects_pools;
    m_AppWindow->SetLeftMenubarCallback([this]()
                                        { this->mainButtonsMenuItem(); });

    // Create project components
    cp_SimpleTable = Cherry::Application::Get().CreateComponent<Cherry::SimpleTable>("simpletable_2", "KeyvA", std::vector<std::string>{"", ""});
    cp_SimpleTable->SetHeaderCellPaddingY(12.0f);
    cp_SimpleTable->SetHeaderCellPaddingX(10.0f);
    cp_SimpleTable->SetRowsCellPaddingY(100.0f);

    v_ProjectName = std::make_shared<std::string>("VortexProject");
    cp_ProjectName = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValString>("keyvaldouble_1", v_ProjectName, "Project name");

    v_ProjectDescription = std::make_shared<std::string>("This is a amazing description of the project.");
    cp_ProjectDescription = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValString>("keyvaldouble_2", v_ProjectDescription, "Description");

    // v_ProjectVersion = std::make_shared<std::string>("1.0.0");
    cp_ProjectVersion = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValSimpleCombo>("keyvaldouble_3", std::vector<std::string>(), 0, "Version");

    v_ProjectAuthor = std::make_shared<std::string>("Your team");
    cp_ProjectAuthor = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValString>("keyvaldouble_4", v_ProjectAuthor, "Authors");

    // v_ProjectAuthor = std::make_shared<std::string>("Your team");
    std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
    loadProjects(projectPoolsPaths, path + "/projects_pools.json");
    cp_ProjectPath = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValSimpleCombo>("keyvaldouble_5", projectPoolsPaths, 0, "Select a path");

    v_ProjectOpen = std::make_shared<bool>(true);
    cp_ProjectOpen = Cherry::Application::Get().CreateComponent<Cherry::DoubleKeyValBoolean>("keyvaldouble_6", v_ProjectOpen, "Open after ?");

    this->ctx = VortexMaker::GetCurrentContext();
    this->Refresh();
}

void ProjectManager::Render()
{
    static bool open_deletion_modal = false;
    static bool user_string_validation = false;
    static char string_validation[256] = "";
    static float leftPaneWidth = 825.0f;
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;
    static int selected;

    if (open_import_projects)
    {
        /*static ImGuiTableFlags window_flags = ImGuiWindowFlags_MenuBar;
        static bool first_time = true;

        static std::shared_ptr<Cherry::ImageTextButtonSimple> button = std::make_shared<Cherry::ImageTextButtonSimple>("Button");

        if (first_time)
        {
            ImGui::SetNextWindowSize(ImVec2(820, 420));
        }

       if (ImGui::BeginPopupModal("Search for project(s) to add", NULL, window_flags))
        {
            if (first_time)
            {
                first_time = false;
            }

            static char path_input_all[512];

            if (ImGui::BeginMenuBar())
            {
                ImGui::Text("Please chose a folder");
                button->Render("");
                ImGui::InputText("###Path", path_input_all, IM_ARRAYSIZE(path_input_all));
                std::string label = "Find###templates";
                if (ImGui::Button(label.c_str()))
                {
                    finded_projects = VortexMaker::FindProjectInFolder(path_input_all);
                }
                ImGui::EndMenuBar();
            }

            if (!finded_projects.empty())
            {
                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))
                if (ImGui::BeginTable("ModulesToAddTable", 4, flags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Vortex version", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    for (int i = 0; i <= finded_projects.size(); i++)
                    {
                        static std::pair<char[128], char[128]> newItem;
                        static char label[128];

                        ImGui::TableNextRow();
                        for (int column = 0; column < 4; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                if (ImGui::Button("Import"))
                                {
                                    // Search if a same name exist, if yes, rename this project
                                    if (false)
                                    {
                                    }
                                    else
                                    {
                                        VortexMaker::ImportProject(path_input_all);
                                    }
                                }
                            }
                            else if (column == 1)
                            {
                                ImGui::Text(finded_projects[i]->name.c_str());
                            }
                            else if (column == 2)
                            {
                                ImGui::Text(finded_projects[i]->author.c_str());
                            }
                            else if (column == 3)
                            {
                                ImGui::Text(finded_projects[i]->compatibleWith.c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize = ImVec2(100, 30);
            ImVec2 bitButtonSize = ImVec2(150, 30);
            float ysection = windowSize.y - 280;
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

            float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

            float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - 10;

            ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

            if (ImGui::Button("Done", ImVec2(120, 0)))
            {

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                showProjectPools = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
   */
    }

    if (open_deletion_modal)
    {
        if (ImGui::BeginPopupModal("Delete a project", NULL, NULL))
        {
            static char path_input_all[512];
            ImGui::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
            ImGui::InputText("Please", string_validation, sizeof(string_validation));
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                open_deletion_modal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if (strcmp(string_validation, selected_envproject_to_remove->name.c_str()) != 0)
            {
                ImGui::BeginDisabled();
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                // Delete
                VortexMaker::DeleteProject(selected_envproject_to_remove->path, selected_envproject_to_remove->name);

                VortexMaker::RefreshEnvironmentProjects();

                open_deletion_modal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(3);
            if (strcmp(string_validation, selected_envproject_to_remove->name.c_str()) != 0)
            {
                ImGui::EndDisabled();
            }
            ImGui::EndPopup();
        }
    }

    if (open_deletion_modal)
        ImGui::OpenPopup("Delete a project");

    if (open_import_projects)
    {
        ImGui::OpenPopup("Import project(s)");

        ImVec2 main_window_size = ImGui::GetWindowSize();
        ImVec2 window_pos = ImGui::GetWindowPos();

        ImGui::SetNextWindowPos(ImVec2(window_pos.x + (main_window_size.x * 0.5f) - 300, window_pos.y + 150));

        ImGui::SetNextWindowSize(ImVec2(600, 0), ImGuiCond_Always);

        if (ImGui::BeginPopupModal("Import project(s)", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
        {
            std::string text_label = "Search projects to import... ";
            ImGui::TextWrapped(text_label.c_str());
            ImGui::TextWrapped("Important: This action will not delete/change selected modules...");
            ImGui::TextWrapped("These modules will be imported:");

            ImGui::Separator();

            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            float max_table_height = 200.0f;
            ImGui::BeginChild("TableScrollRegion", ImVec2(0, max_table_height), true, ImGuiWindowFlags_HorizontalScrollbar);

            static char pathToFolder[512]{};
            ImGui::InputText("Folder", pathToFolder, sizeof(pathToFolder));
            ImGui::SameLine();

            if (ImGui::Button("Search"))
            {
                finded_projects.clear();
                m_StillSearching = true;
                SearchStarted = true;
                VortexMaker::FindpProjectsInDirectoryRecursively(pathToFolder, finded_projects, m_StillSearching, m_ElapsedTime);
            }

            if (m_StillSearching)
            {
                ImGui::Text("Searching....");
                ImGui::SameLine();
                if (ImGui::Button("Stop"))
                {
                    m_StillSearching == false;
                }
            }

            if (!m_StillSearching && SearchStarted)
            {
                std::string label = std::to_string(finded_projects.size()) + " project(s) finded in over " + m_ElapsedTime;
                ImGui::Text(label.c_str());
            }

            static int project_to_import_size = finded_projects.size();

            if (project_to_import_size != finded_projects.size())
            {
                selectedRows.resize(finded_projects.size());
            }

            for (auto &project : finded_projects)
            {
                ProjectImportButton(project);
            }

            if (!finded_projects_to_import.empty())
            {

                std::string label = "Import " + std::to_string(finded_projects_to_import.size()) + " project(s)";

                static std::shared_ptr<Cherry::ImageTextButtonSimple> import_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                import_btn->SetScale(0.85f);
                import_btn->SetInternalMarginX(10.0f);
                import_btn->SetLogoSize(15, 15);
                import_btn->SetBackgroundColorIdle("#00000000");
                import_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"));
                import_btn->SetLabel(label);
                static std::string to_import_destination;

                static std::shared_ptr<Cherry::ComboSimple> combo_dest = std::make_shared<Cherry::ComboSimple>("combo", "Import to", project_pools, 0);
                combo_dest->Render("qd");
                to_import_destination = combo_dest->GetData("selected_string");

                if (import_btn->Render("import_btn"))
                {
                    for (auto &module : finded_projects_to_import)
                    {
                        VortexMaker::ImportProject(module->path, to_import_destination);
                    }

                    VortexMaker::RefreshEnvironmentProjects();

                    finded_projects_to_import.clear();
                    ImGui::CloseCurrentPopup();
                    open_import_projects = false;
                }
            }

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                open_import_projects = false;
            }

            ImGui::EndChild();

            ImGui::Separator();

            ImGui::Separator();

            ImGui::EndPopup();
        }
    }

    if (!project_creation)
    {
        ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        Cherry::TitleFourColored("All projects ", "#75757575");
        for (int row = 0; row < ctx->IO.sys_projects.size(); row++)
        {
            if (areStringsSimilar(ctx->IO.sys_projects[row]->name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
            {
                MyButton(ctx->IO.sys_projects[row]);
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

        ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
        ImGui::BeginChild("###rightpan");
        if (!selected_envproject)
        {
            auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
            auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));

            ImVec2 child_size = ImGui::GetContentRegionAvail();

            ImVec2 image_pos = ImVec2(
                (child_size.x - texture_size.x) / 2.0f,
                (child_size.y - texture_size.y) / 2.0f);

            ImGui::SetCursorPos(image_pos);

            ImGui::Image(texture, texture_size);
        }
        else
        {
            {
                // LOGO Section
                ImGui::BeginChild("LOGO_", ImVec2(80, 80), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 60) * 0.5f); // Center the logo horizontally
                MyButton(selected_envproject->logoPath, 60, 60);
                ImGui::EndChild();
                ImGui::SameLine();
            }

            {
                // Project Info Section
                ImGuiID _id = ImGui::GetID("INFO_PANEL");
                ImGui::BeginChild(_id, ImVec2(0, 100), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
                ImGui::SetCursorPosY(ImGui::GetStyle().ItemSpacing.y);

                float fontScale = 1.3f;
                float oldFontSize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale = fontScale;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), selected_envproject->name.c_str());

                ImGui::GetFont()->Scale = oldFontSize;
                ImGui::PopFont();

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Last opened: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.8f, 0.8f), selected_envproject->lastOpened.c_str());

                ImGui::EndChild();
            }

            // Divider
            ImGui::Separator();
            ImGui::Spacing();

            // Details Section
            {
                ImGui::BeginChild("DETAILS_PANEL", ImVec2(0, 150), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

                auto AddInfoRow = [](const char *label, const std::string &value, const ImVec4 &valueColor = ImVec4(1.0f, 1.0f, 1.0f, 0.9f))
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label);
                    ImGui::SameLine();
                    ImGui::TextColored(valueColor, value.c_str());
                };

                AddInfoRow("Author(s):", selected_envproject->author);
                AddInfoRow("Description:", selected_envproject->description);
                AddInfoRow("Version:", selected_envproject->version);
                AddInfoRow("Compatible with:", selected_envproject->compatibleWith);

                ImGui::EndChild();
            }

            // Footer Buttons
            ImGui::Spacing();
            ImGui::Separator();

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize = ImVec2(120, 35);
            float footerSpacing = 10;
            float buttonPosX = windowSize.x - buttonSize.x * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + footerSpacing);
            ImGui::SetCursorPosX(buttonPosX);

            if (ImGui::Button("Delete", buttonSize))
            {
                selected_envproject_to_remove = selected_envproject;
                open_deletion_modal = true;
            }

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 1.0f, 0.9f));

            if (ImGui::Button("Open Project", buttonSize))
            {
                if (VortexMaker::CheckIfProjectRunning(selected_envproject->path))
                {
                    ImGui::OpenPopup("Project Already Running");
                }
                else
                {
                    VortexMaker::OpenProject(selected_envproject->path, selected_envproject->compatibleWith);
                }
            }

            if (ImGui::BeginPopupModal("Project Already Running", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("The project seems to have already been launched. Would you like to relaunch a new instance?");

                ImGui::Spacing();
                if (ImGui::Button("Retour", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Oui", ImVec2(120, 0)))
                {
                    VortexMaker::OpenProject(selected_envproject->path, selected_envproject->compatibleWith);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::PopStyleColor(3);
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    else
    {
        float columnWidths[3] = {120.0f, 250.0f, 150.0f};

        ImVec2 windowSize = ImGui::GetWindowSize();

        float availableWidth = windowSize.x - ImGui::GetStyle().ItemSpacing.x * 2;

        float totalColumnWidths = 0.0f;
        for (int i = 0; i < 3; ++i)
            totalColumnWidths += columnWidths[i];

        float columnProportions[3];
        for (int i = 0; i < 3; ++i)
            columnProportions[i] = columnWidths[i] / totalColumnWidths;

        ImGui::Columns(3, nullptr, false);

        ImVec4 columnColors[3] = {ImVec4(1.0f, 0.0f, 0.0f, 0.0f),
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.0f),
                                  ImVec4(0.0f, 1.0f, 0.0f, 0.0f)};

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        for (int i = 0; i < 3; ++i)
        {
            float columnWidth = availableWidth * columnProportions[i];
            ImGui::SetColumnWidth(i, columnWidth);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, columnColors[i]);
            ImGui::BeginChild(ImGui::GetID((void *)(intptr_t)i), ImVec2(columnWidth, 0), true);
            if (i == 0)
            {
                MyBanner(Cherry::GetPath("resources/imgs/banners/b_all_bases.png"));
                MyBanner(Cherry::GetPath("resources/imgs/banners/b_operating_systems.png"));
                MyBanner(Cherry::GetPath("resources/imgs/banners/b_app_svc.png"));
                MyBanner(Cherry::GetPath("resources/imgs/banners/b_tools_utils.png"));
                MyBanner(Cherry::GetPath("resources/imgs/banners/b_assets_components.png"));
            }

            else if (i == 1)
            {
                for (int project = 0; project < project_templates.size(); project++)
                {
                    if (project_templates[project] != NULL)
                    {
                        MyButton(project_templates[project]);
                    }
                }
            }
            else if (i == 2)
            {
                if (!selected_template_object)
                {
                    auto texture = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));
                    auto texture_size = Cherry::GetTextureSize(Cherry::GetPath("resources/imgs/icons/misc/frame_selectproject.png"));

                    ImVec2 child_size = ImGui::GetContentRegionAvail();

                    ImVec2 image_pos = ImVec2(
                        (child_size.x - texture_size.x) / 2.0f,
                        (child_size.y - texture_size.y) / 2.0f);

                    ImGui::SetCursorPos(image_pos);

                    ImGui::Image(texture, texture_size);
                }
                else
                {
                    ImVec2 windowSize = ImGui::GetWindowSize();
                    ImVec2 contentSize = ImGui::GetContentRegionAvail();
                    ImVec2 buttonSize = ImVec2(100, 30);
                    ImVec2 bitButtonSize = ImVec2(150, 30);

                    static char buf[255]{};
                    std::string s{VortexMaker::getHomeDirectory() + "/VortexProjects/"};

                    strncpy(buf, s.c_str(), sizeof(buf) - 1);

                    static char name[128] = "UnknowName";
                    static char desc[128] = "My project description !";
                    static char version[128] = "1.0.0";
                    static char auth[128] = "you";
                    static bool open = true;
                    {
                        ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                        if (!selected_template_object->m_logo_path.empty())
                        {
                            MyButton(selected_template_object->m_logo_path, 60, 60);
                        }

                        ImGui::EndChild();
                        ImGui::SameLine();
                    }

                    {
                        ImGui::BeginChild("TEXT_", ImVec2(0, 68), true);
                        float oldsize = ImGui::GetFont()->Scale;
                        ImGui::GetFont()->Scale *= 1.3;
                        ImGui::PushFont(ImGui::GetFont());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), cp_ProjectName->GetData("value").c_str());
                        ImGui::SameLine();

                        ImGui::GetFont()->Scale = oldsize;
                        ImGui::PopFont();
                        static std::string _name = " based on " + selected_template_object->m_proper_name;
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), _name.c_str());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "from");
                        ImGui::SameLine();
                        ImGui::Text(auth);
                        ImGui::EndChild();
                    }

                    ImGui::PopStyleVar(4);
                    /*ImGui::InputText("Path", buf, sizeof(buf));
                    ImGui::InputText("Name", name, 128);
                    ImGui::InputText("Description", desc, 128);
                    ImGui::InputText("Authors", auth, 128);
                    ImGui::InputText("Version", version, 128);*/

                    std::vector<Cherry::SimpleTable::SimpleTableRow> keyvals;

                    keyvals.push_back(Cherry::SimpleTable::SimpleTableRow({[&]()
                                                                           { cp_ProjectName->Render(0); },
                                                                           [&]()
                                                                           { cp_ProjectName->Render(1); }}));

                    keyvals.push_back(Cherry::SimpleTable::SimpleTableRow({[&]()
                                                                           { cp_ProjectDescription->Render(0); },
                                                                           [&]()
                                                                           { cp_ProjectDescription->Render(1); }}));

                    keyvals.push_back(Cherry::SimpleTable::SimpleTableRow({[&]()
                                                                           { cp_ProjectAuthor->Render(0); },
                                                                           [&]()
                                                                           { cp_ProjectAuthor->Render(1); }}));

                    cp_ProjectVersion->SetList(selected_template_object->m_compatible_versions);

                    keyvals.push_back(Cherry::SimpleTable::SimpleTableRow({[&]()
                                                                           { cp_ProjectVersion->Render(0); },
                                                                           [&]()
                                                                           { cp_ProjectVersion->Render(1); }}));

                    keyvals.push_back(Cherry::SimpleTable::SimpleTableRow({[&]()
                                                                           { cp_ProjectPath->Render(0); },
                                                                           [&]()
                                                                           { cp_ProjectPath->Render(1); }}));

                    keyvals.push_back(Cherry::SimpleTable::SimpleTableRow({[&]()
                                                                           { cp_ProjectOpen->Render(0); },
                                                                           [&]()
                                                                           { cp_ProjectOpen->Render(1); }}));
                    cp_SimpleTable->Render(keyvals, "AllParams", ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoBordersInBodyUntilResize);

                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                    std::string path = cp_ProjectPath->GetData("selected_string") + "/" + cp_ProjectName->GetData("value");
                    std::cout << path << std::endl;

                    float firstButtonPosX = windowSize.x - buttonSize.x - bitButtonSize.y - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

                    float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

                    ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

                    if (ImGui::Button("Settings", buttonSize))
                    {
                    }

                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 1.0f, 0.8f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 1.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.0f, 1.8f));
                    if (ImGui::Button("Create new project", bitButtonSize))
                    {
                        VortexMaker::CreateProject(cp_ProjectName->GetData("value"), auth, cp_ProjectVersion->GetData("selected_string"), desc, path, path + "/icon.png", selected_template_object->m_name);
                        VortexMaker::RefreshEnvironmentProjects();

                        if (open)
                        {
                            // TODO
                        }
                    }
                    ImGui::PopStyleColor(3);
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();

            if (i < 2)
                ImGui::NextColumn();
        }
        ImGui::PopStyleVar(4);
    }

    static std::vector<std::string> projectPaths;
    static char newPath[256] = "";
    static bool initialized = false;
    if (!initialized)
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
        std::string json_file = path + "/projects_pools.json";
        loadProjects(projectPaths, json_file);
        initialized = true;
    }

    if (showProjectPools)
    {
        ImGui::OpenPopup("Manage Project Pools");
        if (ImGui::BeginPopupModal("Manage Project Pools", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Path where Vortex search projects:");
            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
            if (ImGui::BeginTable("##project_paths", 2))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

                ImGui::TableSetupColumn("Path");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < projectPaths.size(); ++i)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Text("%s", projectPaths[i].c_str());

                    ImGui::PopStyleColor();

                    static std::shared_ptr<Cherry::ImageTextButtonSimple> import_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                    import_btn->SetScale(0.85f);
                    import_btn->SetInternalMarginX(10.0f);
                    import_btn->SetLogoSize(15, 15);
                    import_btn->SetBackgroundColorIdle("#00000000");
                    import_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));

                    ImGui::TableSetColumnIndex(1);
                    std::string delete_btn_label = "Delete####" + projectPaths[i];
                    if (import_btn->Render(delete_btn_label.c_str()))
                    {
                        projectPaths.erase(projectPaths.begin() + i);
                        --i;
                    }
                }

                ImGui::PopStyleVar();
                ImGui::EndTable();
            }

            ImGui::PopStyleColor();

            static std::shared_ptr<Cherry::ImageTextButtonSimple> add_btn = std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "####add");
            add_btn->SetScale(0.85f);
            add_btn->SetInternalMarginX(10.0f);
            add_btn->SetLogoSize(15, 15);
            add_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));

            static std::shared_ptr<Cherry::ImageTextButtonSimple> save_btn = std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "Save");
            save_btn->SetScale(0.85f);
            save_btn->SetInternalMarginX(10.0f);
            save_btn->SetLogoSize(15, 15);
            save_btn->SetBackgroundColorIdle("#3232F7FF");
            save_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"));

            ImGui::Separator();

            if (add_btn->Render("add_pool"))
            {
                if (newPath[0] != '\0')
                {
                    projectPaths.push_back(std::string(newPath));
                    newPath[0] = '\0';
                }
            }

            ImGui::SameLine();

            ImGui::InputText("", newPath, sizeof(newPath));

            ImGui::Separator();

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                showProjectPools = false;
            }
            ImGui::SameLine();

            if (save_btn->Render("save_pool"))
            {
                std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
                std::string json_file = path + "/projects_pools.json";
                saveProjects(projectPaths, json_file);

                VortexMaker::RefreshEnvironmentProjectsPools();
                VortexMaker::RefreshEnvironmentProjects();
            }
            ImGui::EndPopup();
        }
    }
}

std::shared_ptr<Cherry::AppWindow> &ProjectManager::GetAppWindow()
{
    return m_AppWindow;
}

std::shared_ptr<ProjectManager> ProjectManager::Create(const std::string &name)
{
    auto instance = std::shared_ptr<ProjectManager>(new ProjectManager(name));
    instance->SetupRenderCallback();
    return instance;
}

void ProjectManager::SetupRenderCallback()
{
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]()
                                   {
            if (self) {
                self->Render();
            } });
}

// Helper functions for menu items

static void handleRefresh()
{
    // Behavior
}

static void handleAddToProject(const std::string &name, const std::string &version)
{
    // Behavior
}

static void handleFilterBuildRebuild()
{
    // Behavior
}

static void handleGlobalBuild()
{
    // Behavior
}

static void handleCreateModule()
{
    // Behavior
}

static void handleSearch()
{
    // Behavior
}

void ProjectManager::addModuleModal()
{
    //
}

void ProjectManager::mainButtonsMenuItem()
{
    static std::shared_ptr<Cherry::ImageTextButtonSimple> create_project_button = std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "Create a project");
    create_project_button->SetScale(0.85f);
    create_project_button->SetInternalMarginX(10.0f);
    create_project_button->SetLogoSize(15, 15);
    create_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));

    static std::shared_ptr<Cherry::ImageTextButtonSimple> import_project_button = std::make_shared<Cherry::ImageTextButtonSimple>("import_btn", "Import a project");
    import_project_button->SetScale(0.85f);
    import_project_button->SetInternalMarginX(10.0f);
    import_project_button->SetLogoSize(15, 15);
    import_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"));

    static std::shared_ptr<Cherry::ImageTextButtonSimple> open_project_button = std::make_shared<Cherry::ImageTextButtonSimple>("open_btn", "Open a project");
    open_project_button->SetScale(0.85f);
    open_project_button->SetInternalMarginX(10.0f);
    open_project_button->SetLogoSize(15, 15);
    open_project_button->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"));

    static std::shared_ptr<Cherry::ImageTextButtonSimple> add_pools_btn = std::make_shared<Cherry::ImageTextButtonSimple>("add_pools_btn", "Search folders");
    add_pools_btn->SetScale(0.85f);
    add_pools_btn->SetInternalMarginX(10.0f);
    add_pools_btn->SetLogoSize(15, 15);
    add_pools_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"));

    static std::shared_ptr<std::string> v_SearchString = std::make_shared<std::string>("");
    static std::shared_ptr<Cherry::ImageStringInput> input_search = std::make_shared<Cherry::ImageStringInput>("open_btn", v_SearchString, Cherry::GetPath("resources/imgs/icons/misc/icon_search.png"), "####Open a project");

    strncpy(ProjectSearch, v_SearchString->c_str(), sizeof(ProjectSearch) - 1);

    if (!project_creation)
    {
        if (create_project_button->Render())
        {
            project_creation = true;
        }

        if (import_project_button->Render("import"))
        {
            open_import_projects = true;
        }

        ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#45454545")),
            ImGui::Separator();
        ImGui::PopStyleColor();

        if (add_pools_btn->Render("fq"))
        {
            showProjectPools = true;
        }

        ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#45454545")),
            ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::PushItemWidth(200);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 6.0f)); // Largeur x Hauteur padding
        ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#353535FF"));

        input_search->Render("_");
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        // TODO ProjectSearch = v_SearchString->c_str();
    }
    else
    {
        if (open_project_button->Render())
        {
            project_creation = false;
        }
    }
}

void ProjectManager::filterMenuItem()
{
    ImGui::Separator();
    if (ImGui::BeginMenu("Filters"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleFilterBuildRebuild();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectManager::createMenuItem()
{
    if (ImGui::BeginMenu("Create a module"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleCreateModule();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectManager::searchMenuItem()
{
    if (ImGui::BeginMenu("Search"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleSearch();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectManager::Refresh()
{
    project_templates.clear();
    for (auto tem : this->ctx->IO.sys_templates)
    {
        if (tem->m_type == "project")
        {
            project_templates.push_back(tem);
        }
    }
};