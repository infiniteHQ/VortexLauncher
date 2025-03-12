#include "./download_center.hpp"

// Add CFE for downloadable contents.

DownloadCenter::DownloadCenter(const std::string &name)
{
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");

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

    // Make tabs !! Not childs
    this->AddChild("Vortex versions", [this]()
                   {
    VxContext &ctx = *CVortexMaker;
                       CherryKit::TitleFour("Install latest version"); // 75757575
                       ImGui::SameLine();
                       CherryKit::TitleFour("See all versions available "); // 75757575

                       CherryKit::TitleFour("All installed version "); // 75757575
                       for (int row = 0; row < ctx.IO.sys_vortex_version.size(); row++)
                       {
                           if (areStringsSimilar(ctx.IO.sys_vortex_version[row]->name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                           {
                               VersionButton(ctx.IO.sys_vortex_version[row]->name, 300, 100, ctx.IO.sys_vortex_version[row]->version);
                           }
                       } });
    this->AddChild("System modules", [this]()
                   {
                       //
                   });
    this->AddChild("System plugins", [this]()
                   {
                       //
                   });
    this->AddChild("System templates", [this]()
                   {
                       //
                   });
    this->AddChild("Marketplace", [this]()
                   {
                       //
                   });
    this->AddChild("Account", [this]()
                   {
                       //
                   });

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
    m_AppWindow->SetLeftMenubarCallback([]() {});
    m_AppWindow->SetRightMenubarCallback([win]() {});
}

void DownloadCenter::AddChild(const std::string &child_name, const std::function<void()> &child)
{
    m_Childs[child_name] = child;
}

void DownloadCenter::RemoveChild(const std::string &child_name)
{
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end())
    {
        m_Childs.erase(it);
    }
}

std::function<void()> DownloadCenter::GetChild(const std::string &child_name)
{
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end())
    {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Cherry::AppWindow> &DownloadCenter::GetAppWindow()
{
    return m_AppWindow;
}

std::shared_ptr<DownloadCenter> DownloadCenter::Create(const std::string &name)
{
    auto instance = std::shared_ptr<DownloadCenter>(new DownloadCenter(name));
    instance->SetupRenderCallback();
    return instance;
}

void DownloadCenter::SetupRenderCallback()
{
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]()
                                   {
            if (self) {
                self->Render();
            } });
}

void DownloadCenter::Render()
{
    static float leftPaneWidth = 300.0f;
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;
    static int selected;

    ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

    CherryKit::TitleThree("");

    for (const auto &child : m_Childs)
    {
        if (child.first == m_SelectedChildName)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        if (CherryKit::ButtonText(child.first.c_str()))
        {
            m_SelectedChildName = child.first;
        }

        ImGui::PopStyleColor();
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
