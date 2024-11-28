#include "./main_settings.hpp"

#include <string>
#include <vector>
#include <fstream>

std::string GetConfigFilePath()
{
    std::string homeDir = VortexMaker::getHomeDirectory();
    std::string configPath;
    if (VortexMaker::IsWindows())
    {
        configPath = homeDir + "\\.vx\\configs\\dists.json";
    }
    else
    {
        configPath = homeDir + "/.vx/configs/dists.json";
    }
    return configPath;
}

nlohmann::json LoadConfig(const std::string &filePath)
{
    std::ifstream file(filePath);
    nlohmann::json config;
    if (file.is_open())
    {
        file >> config;
    }
    else
    {
        config = {{"vortex_dists", {"def"}}, {"vortexlauncher_dist", "def"}};
    }
    return config;
}

void SaveConfig(const std::string &filePath, const nlohmann::json &config)
{
    std::ofstream file(filePath);
    if (file.is_open())
    {
        file << config.dump(4);
    }
}

static std::vector<std::string> projectPoolsPaths;
static std::vector<std::string> modulesPoolsPaths;
static std::vector<std::string> templatesPoolsPaths;
static std::vector<std::string> vortexVersionPoolsPaths;

namespace VortexLauncher
{
    void MainSettings::RefreshConfig()
    {
        auto config = LoadConfig(GetConfigFilePath());
        vortexDists = config["vortex_dists"].get<std::vector<std::string>>();
        vortexLauncherDist = config["vortexlauncher_dist"].get<std::string>();
    }

    void MainSettings::SaveCurrentConfig()
    {
        nlohmann::json config;
        config["vortex_dists"] = vortexDists;
        config["vortexlauncher_dist"] = vortexLauncherDist;
        SaveConfig(GetConfigFilePath(), config);
    }

    MainSettings::MainSettings(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

        RefreshConfig();
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

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        this->AddChild("Paths, Links", "Templates pools paths", [this]()
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

        this->AddChild("Paths, Links", "Modules pools paths", [this]()
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

        this->AddChild("Paths, Links", "Projects pools paths", [this]()
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

        this->AddChild("Paths, Links", "Vortex versions paths", [this]()
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
            } 
            
        
            ImGui::PopStyleColor(); });

        this->AddChild("Distribution", "Vortex distribution", [this]()
                       {
 ImGui::Text("Vortex Distributions:");
    if (ImGui::BeginTable("##vortex_dists", 2)) {
        ImGui::TableSetupColumn("Path");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();
std::cout << vortexDists.size() << std::endl;
        for (size_t i = 0; i < vortexDists.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", vortexDists[i].c_str());

            ImGui::TableSetColumnIndex(1);
            if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                vortexDists.erase(vortexDists.begin() + i);
                --i;
            }
        }
        ImGui::EndTable();
    }

    ImGui::InputText("New Distribution", newDist, 256);
    if (ImGui::Button("Add####ADDTODISTRIB")) {
            vortexDists.push_back(newDist);
        }

    ImGui::Separator();

    if (ImGui::Button("Save")) {
        SaveCurrentConfig();
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        RefreshConfig();
    } });

        this->AddChild("Distribution", "Vortex Launcher distribution", [this]()
                       {
                           ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#353535FF"));
                           ImGui::Text("Launcher Distribution:");
                           if (ImGui::BeginCombo("##vortexlauncher_dist", vortexLauncherDist.c_str()))
                           {
                               const char *options[] = {"stable", "beta", "testing"};
                               for (const char *option : options)
                               {
                                   bool isSelected = (vortexLauncherDist == option);
                                   if (ImGui::Selectable(option, isSelected))
                                   {
                                       vortexLauncherDist = option;
                                   }
                                   if (isSelected)
                                   {
                                       ImGui::SetItemDefaultFocus();
                                   }
                               }
                               ImGui::EndCombo();
                           }

                           ImGui::Separator();

                           if (ImGui::Button("Save"))
                           {
                               SaveCurrentConfig();
                           }
                           ImGui::SameLine();
                           if (ImGui::Button("Refresh"))
                           {
                               RefreshConfig();
                           }
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
        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;
        std::map<std::string, std::vector<MainSettingsChild>> groupedByParent;
        for (const auto &child : m_Childs)
        {
            groupedByParent[child.m_Parent].push_back(child);
        }

        std::string label = "left_pane" + m_AppWindow->m_Name;
        ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

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

}
