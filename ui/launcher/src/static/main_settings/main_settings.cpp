#include "./main_settings.hpp"

static std::vector<std::string> projectPoolsPaths;
static std::vector<std::string> modulesPoolsPaths;
static std::vector<std::string> templatesPoolsPaths;
static std::vector<std::string> vortexVersionPoolsPaths;

namespace VortexLauncher
{
    MainSettings::MainSettings(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

        std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";

        loadProjects(projectPoolsPaths, path + "/projects_pools.json");
        loadModules(modulesPoolsPaths, path + "/modules_pools.json");
        loadTemplates(templatesPoolsPaths, path + "/templates_pools.json");
        loadVortexVersions(vortexVersionPoolsPaths, path + "/vortex_versions_pools.json");

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

        this->AddChild("Contents", "Templates pools paths", [this]()
                       {
            static char newPath[256] = "";

            ImGui::Text("Path where Vortex search modules on system:");
            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
            if (ImGui::BeginTable("##project_paths", 2))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

                ImGui::TableSetupColumn("Path");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < templatesPoolsPaths.size(); ++i)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Text("%s", templatesPoolsPaths[i].c_str());

                    ImGui::PopStyleColor();

                    static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                    del_btn->SetScale(0.85f);
                    del_btn->SetInternalMarginX(10.0f);
                    del_btn->SetLogoSize(15, 15);
                    del_btn->SetBackgroundColorIdle("#00000000");
                    del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));

                    ImGui::TableSetColumnIndex(1);
                    std::string delete_btn_label = "Delete####" + templatesPoolsPaths[i];
                    if (del_btn->Render(delete_btn_label.c_str()))
                    {
                        templatesPoolsPaths.erase(templatesPoolsPaths.begin() + i);
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
                    templatesPoolsPaths.push_back(std::string(newPath));
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
                std::string json_file = path + "/template_pools.json";
                saveTemplates(templatesPoolsPaths, json_file);

                VortexMaker::RefreshEnvironmentProjectsPools();
                VortexMaker::RefreshEnvironmentProjects();
            } });

        this->AddChild("Contents", "Modules pools paths", [this]()
                       {
            static char newPath[256] = "";

            ImGui::Text("Path where Vortex search modules on system:");
            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
            if (ImGui::BeginTable("##project_paths", 2))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

                ImGui::TableSetupColumn("Path");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < modulesPoolsPaths.size(); ++i)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Text("%s", modulesPoolsPaths[i].c_str());

                    ImGui::PopStyleColor();

                    static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                    del_btn->SetScale(0.85f);
                    del_btn->SetInternalMarginX(10.0f);
                    del_btn->SetLogoSize(15, 15);
                    del_btn->SetBackgroundColorIdle("#00000000");
                    del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));

                    ImGui::TableSetColumnIndex(1);
                    std::string delete_btn_label = "Delete####" + modulesPoolsPaths[i];
                    if (del_btn->Render(delete_btn_label.c_str()))
                    {
                        modulesPoolsPaths.erase(modulesPoolsPaths.begin() + i);
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
                    modulesPoolsPaths.push_back(std::string(newPath));
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
                std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
                std::string json_file = path + "/modules_pools.json";
                saveModules(modulesPoolsPaths, json_file);

                VortexMaker::RefreshEnvironmentProjectsPools();
                VortexMaker::RefreshEnvironmentProjects();
            } });

        this->AddChild("Contents", "Projects pools paths", [this]()
                       {
            static char newPath[256] = "";

            ImGui::Text("Path where Vortex search projects:");
            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
            if (ImGui::BeginTable("##project_paths", 2))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

                ImGui::TableSetupColumn("Path");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < projectPoolsPaths.size(); ++i)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Text("%s", projectPoolsPaths[i].c_str());

                    ImGui::PopStyleColor();

                    static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                    del_btn->SetScale(0.85f);
                    del_btn->SetInternalMarginX(10.0f);
                    del_btn->SetLogoSize(15, 15);
                    del_btn->SetBackgroundColorIdle("#00000000");
                    del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));

                    ImGui::TableSetColumnIndex(1);
                    std::string delete_btn_label = "Delete####" + projectPoolsPaths[i];
                    if (del_btn->Render(delete_btn_label.c_str()))
                    {
                        projectPoolsPaths.erase(projectPoolsPaths.begin() + i);
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
                    projectPoolsPaths.push_back(std::string(newPath));
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
                std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
                std::string json_file = path + "/projects_pools.json";
                saveProjects(projectPoolsPaths, json_file);

                VortexMaker::RefreshEnvironmentProjectsPools();
                VortexMaker::RefreshEnvironmentProjects();
            } });

        this->AddChild("Contents", "Vortex versions paths", [this]()
                       {

            static char newPath[256] = "";
            Cherry::TitleSixColored("Important: It's not recommanded to add other pools for Vortex versions than the unique system pool.", "#F63535FF");

            ImGui::Text("Path where Vortex search versions:");
            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, Cherry::HexToRGBA("#151515FF"));
            if (ImGui::BeginTable("##project_paths", 2))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

                ImGui::TableSetupColumn("Path");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < vortexVersionPoolsPaths.size(); ++i)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Text("%s", vortexVersionPoolsPaths[i].c_str());

                    ImGui::PopStyleColor();

                    static std::shared_ptr<Cherry::ImageTextButtonSimple> del_btn = std::make_shared<Cherry::ImageTextButtonSimple>("delete_project_pool_button", "");
                    del_btn->SetScale(0.85f);
                    del_btn->SetInternalMarginX(10.0f);
                    del_btn->SetLogoSize(15, 15);
                    del_btn->SetBackgroundColorIdle("#00000000");
                    del_btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_delete.png"));

                    ImGui::TableSetColumnIndex(1);
                    std::string delete_btn_label = "Delete####" + vortexVersionPoolsPaths[i];
                    if (del_btn->Render(delete_btn_label.c_str()))
                    {
                        vortexVersionPoolsPaths.erase(vortexVersionPoolsPaths.begin() + i);
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
                    vortexVersionPoolsPaths.push_back(std::string(newPath));
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
                std::string path = VortexMaker::getHomeDirectory() + "/.vx/configs/";
                std::string json_file = path + "/vortex_versions.json";
                saveVortexVersions(vortexVersionPoolsPaths, json_file);

                VortexMaker::RefreshEnvironmentProjectsPools();
                VortexMaker::RefreshEnvironmentProjects();
            } });

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

    std::shared_ptr<Cherry::AppWindow> &MainSettings::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<MainSettings> MainSettings::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<MainSettings>(new MainSettings(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void MainSettings::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void MainSettings::Render()
    {
        static float leftPaneWidth = 300.0f;
        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;
        static int selected;
        std::map<std::string, std::vector<MainSettingsChild>> groupedByParent;
        for (const auto &child : m_Childs)
        {
            groupedByParent[child.m_Parent].push_back(child);
        }

        ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

        Cherry::TitleThree("Settings of Vortex");
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

                if (Cherry::TextButtonUnderline(child.m_ChildName.c_str()))
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

}
