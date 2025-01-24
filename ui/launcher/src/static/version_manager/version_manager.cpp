#include "./version_manager.hpp"
using namespace Cherry;

static std::vector<std::shared_ptr<ModuleInterface>> c_FindedModules;
static std::vector<std::shared_ptr<ModuleInterface>> to_suppr_modules;
static std::vector<std::shared_ptr<ModuleInterface>> to_import_modules;

static bool showModulesDeletionModal = false;
static bool showModulesImportModal = false;
static bool showPluginsDeletionModal = false;
static bool showPluginsImportModal = false;

static void SearchModulesOnDirectory(const std::string &path)
{
    c_FindedModules = VortexMaker::FindModulesInDirectory(path);
}

VersionManagerAppWindow::VersionManagerAppWindow(const std::string &name)
{
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    m_AppWindow->SetVisibility(false);
    m_AppWindow->SetCloseCallback([this]()
                                  { m_AppWindow->SetVisibility(false); });

    /*m_FileBrowser = std::make_shared<FileBrowserAppWindow>("Search Folder", VortexMaker::getHomeDirectory());
    m_FileBrowser->RefreshRender(m_FileBrowser);
    m_FileBrowser->GetAppWindow()->SetVisibility(false);

    Cherry::AddAppWindow(m_FileBrowser->GetAppWindow());*/

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
    this->m_SelectedChildName = "Manage Vortex versions";

    this->AddChild("Summary", "Manage Vortex versions", [this]()
                   {
    VxContext &ctx = *CVortexMaker;
                       Cherry::TitleFourColored("Quick actions", "#75757575");

                       if(ctx.disconnected)
                       {
                        VersionButton("Latest", 300, 100, "?", Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"));
                        VersionButton("All Versions", 300, 100, "", Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"));
                       }
                       else
                       {
                        DownloadableVersionButton("Latest", 300, 100, ctx.latest_vortex_version.version, Cherry::GetPath("resources/imgs/vortex_latest.png"));
                        VersionButton("All Versions", 300, 100, "", Cherry::GetPath("resources/imgs/vortex_versions.png"), false, [this](){m_SelectedChildName = "Download new Vortex versions";});
                       }

                       ImGui::Text("");
                       ImGui::Spacing();
                       ImGui::Spacing();
                       ImGui::Spacing();
                       ImGui::Spacing();

                       Cherry::TitleFourColored("All installed versions", "#75757575");

if(ctx.IO.sys_vortex_version.size() <= 0)
{
    VersionButton("", 300, 100, "", Cherry::GetPath("resources/imgs/no_versions.png"));
}
                       for (int row = 0; row < ctx.IO.sys_vortex_version.size(); row++)
                       {
                           if (areStringsSimilar(ctx.IO.sys_vortex_version[row]->name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                           {                               
                               InstalledVersionButton(ctx.IO.sys_vortex_version[row]->path, ctx.IO.sys_vortex_version[row]->name, 300, 100, ctx.IO.sys_vortex_version[row]->version, ctx.IO.sys_vortex_version[row]->banner);
                           }
                       } });

    this->AddChild("Vortex", "Download new Vortex versions", [this]()
                   {
    VxContext &ctx = *CVortexMaker;
                       if(ctx.disconnected)
                       {
                        VersionButton("???", 300, 100, "?", Cherry::GetPath("resources/imgs/vortex_banner_disconnected.png"));
                       }
                       else
                       {
                       Cherry::TitleFourColored("All available Vortex versions", "#75757575");

if(ctx.latest_vortex_versions.size() <= 0)
{
    VersionButton("", 250, 120, "", Cherry::GetPath("resources/imgs/no_available_versions.png"));
}

                       for (auto version : VortexMaker::GetCurrentContext()->latest_vortex_versions)
                       {
                           if (areStringsSimilar(version.name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                           {
                           DownloadableVersionButton(version.name, 300, 100, version.version, Cherry::GetHttpPath(version.banner), "none", version.dist, version.arch, version.plat);
                            }

                       }
                       } });

    this->AddChild("Vortex", "Import Vortex versions", [this]()
                   {


Cherry::TitleFourColored("All installed versions", "#75757575");

    VxContext &ctx = *CVortexMaker;

                       for (int row = 0; row < ctx.IO.sys_vortex_version.size(); row++)
                       {
                           if (areStringsSimilar(ctx.IO.sys_vortex_version[row]->name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                           {
                               VersionButton(ctx.IO.sys_vortex_version[row]->name, 300, 100, ctx.IO.sys_vortex_version[row]->version, Cherry::GetPath(ctx.IO.sys_vortex_version[row]->banner));
                           }
                       } });

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
    m_AppWindow->SetLeftMenubarCallback([]() {});
    m_AppWindow->SetRightMenubarCallback([win]() {});
}

void VersionManagerAppWindow::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
{
    m_Childs.push_back(VersionManagerAppWindowChild(parent_name, child_name, child));
}

void VersionManagerAppWindow::RemoveChild(const std::string &child_name)
{
}

std::shared_ptr<Cherry::AppWindow> &VersionManagerAppWindow::GetAppWindow()
{
    return m_AppWindow;
}

std::shared_ptr<VersionManagerAppWindow> VersionManagerAppWindow::Create(const std::string &name)
{
    auto instance = std::shared_ptr<VersionManagerAppWindow>(new VersionManagerAppWindow(name));
    instance->SetupRenderCallback();
    return instance;
}

void VersionManagerAppWindow::SetupRenderCallback()
{
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]()
                                   {
            if (self) {
                self->Render();
            } });
}

std::function<void()> VersionManagerAppWindow::GetChild(const std::string &child_name)
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

void VersionManagerAppWindow::Render()
{
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;
    std::map<std::string, std::vector<VersionManagerAppWindowChild>> groupedByParent;
    for (const auto &child : m_Childs)
    {
        groupedByParent[child.m_Parent].push_back(child);
    }
    std::string label = "left_pane" + m_AppWindow->m_Name;
    ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

    TitleThree("Manage Vortex version(s)");
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
