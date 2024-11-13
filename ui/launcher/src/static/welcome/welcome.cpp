#include "./welcome.hpp"

namespace VortexLauncher
{
    static void HomeBanner(const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &version = "?", const std::string &path = "resources/imgs/vortex_banner_unknow.png")
    {
        ImVec2 squareSize(xsize, ysize);

        const char *originalText = envproject.c_str();
        char truncatedText[32];
        const char *versionText = version.c_str();

        if (strlen(originalText) > 24)
        {
            strncpy(truncatedText, originalText, 8);
            strcpy(truncatedText + 8, "...");
        }
        else
        {
            strcpy(truncatedText, originalText);
        }

        ImVec2 textSize = ImGui::CalcTextSize(truncatedText);
        ImVec2 totalSize(squareSize.x, squareSize.y + textSize.y + 5);

        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        std::string button_id = envproject + "squareButtonWithText" + envproject;
        if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
        {
            // selected_envproject = envproject;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        ImDrawList *drawList = ImGui::GetWindowDrawList();

        if (!envproject.empty() && std::filesystem::exists(envproject))
        {
            drawList->AddImage(Cherry::GetTexture(envproject), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
        }
        else
        {
            drawList->AddImage(Cherry::GetTexture(Cherry::GetPath(path)), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
        }

        ImVec2 smallRectSize(40, 20);
        ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

        drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
        ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(versionText).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);
        drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);

        ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);

        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            drawList->AddRect(cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y), IM_COL32(135, 135, 135, 255), 0.0f, 0, 2.0f);
        }

        // DrawHighlightedText(drawList, textPos, truncatedText, ProjectSearch, highlightColor, textColor, highlightTextColor);

        float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        if (cursorPos.x + totalSize.x < windowVisibleX2)
            ImGui::SameLine();
    }

static void NewsBanner(bool disable_stack, const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &version = "?", const std::string &path = "resources/imgs/vortex_banner_unknow.png")
{
    ImVec2 squareSize(xsize, ysize);

    const char *originalText = envproject.c_str();
    char truncatedText[32];
    const char *versionText = version.c_str();

    if (strlen(originalText) > 24)
    {
        strncpy(truncatedText, originalText, 8);
        strcpy(truncatedText + 8, "...");
    }
    else
    {
        strcpy(truncatedText, originalText);
    }

    ImVec2 textSize = ImGui::CalcTextSize(truncatedText);
    ImVec2 totalSize(squareSize.x, squareSize.y + textSize.y + 5);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    std::string button_id = envproject + "squareButtonWithText" + envproject;
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
        // selected_envproject = envproject;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    if (!envproject.empty() && std::filesystem::exists(envproject))
    {
        drawList->AddImage(Cherry::GetTexture(envproject), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }
    else
    {
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath(path)), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
    ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(versionText).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);

    // Calculate the wrapping position
    ImVec2 textPos = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + 5); // Adjust position to start below square
    std::string fullText = "TEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newxsTEst newx";
    float maxWidth = squareSize.x; // Define max width for wrapping

    // Wrap text if it exceeds square width
    std::string line;
    float lineWidth = 0.0f;
    for (char ch : fullText)
    {
        ImVec2 charSize = ImGui::CalcTextSize(std::string(1, ch).c_str());
        if (lineWidth + charSize.x > maxWidth && !line.empty())
        {
            // Draw current line, reset for next
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), line.c_str());
            textPos.y += charSize.y; // Move down for next line
            line.clear();
            lineWidth = 0.0f;
        }
        line += ch;
        lineWidth += charSize.x;
    }
    // Draw any remaining text
    if (!line.empty())
    {
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), line.c_str());
    }

    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y), IM_COL32(135, 135, 135, 255), 0.0f, 0, 2.0f);
    }

    if (!disable_stack)
    {
        float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        if (cursorPos.x + totalSize.x < windowVisibleX2)
            ImGui::SameLine();
    }
}

    
    WelcomeWindow::WelcomeWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));
        m_AppWindow->SetClosable(false);

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

        m_SelectedChildName = "?loc:loc.windows.welcome.overview";

        this->AddChild("?loc:loc.windows.welcome.sponsor", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.windows.welcome.contribute", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.windows.welcome.fast_actions", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.windows.welcome.trends", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.windows.welcome.actuality", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.windows.welcome.whats_news", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.windows.welcome.overview", [this]()
                       {
                           float childWidth = ImGui::GetContentRegionAvail().x;
                           float inputTextWidth = 300.0f;
                           float buttonWidth = 100.0f;
                           float groupWidth = inputTextWidth + ImGui::GetStyle().ItemSpacing.x + buttonWidth;


                           // News
                           Cherry::TitleFive("Latest news");
                           NewsBanner(false, "Latest", 380, 150, "?", "resources/imgs/vortex_banner_disconnected.png");
                           NewsBanner(true, "All Versions", 380, 150, "", "resources/imgs/vortex_banner_disconnected.png");

                           Cherry::TitleFive("Latest openned project");
                           Cherry::TitleFive("Latest openned project");

                           //
                       });

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
    }

    void WelcomeWindow::AddChild(const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs[child_name] = child;
    }

    void WelcomeWindow::RemoveChild(const std::string &child_name)
    {
        auto it = m_Childs.find(child_name);
        if (it != m_Childs.end())
        {
            m_Childs.erase(it);
        }
    }

    std::shared_ptr<Cherry::AppWindow> &WelcomeWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<WelcomeWindow> WelcomeWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<WelcomeWindow>(new WelcomeWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void WelcomeWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    std::function<void()> WelcomeWindow::GetChild(const std::string &child_name)
    {
        auto it = m_Childs.find(child_name);
        if (it != m_Childs.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void WelcomeWindow::Render()
    {
        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;

        std::string label = "left_pane" + m_AppWindow->m_Name;
        ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        Cherry::TitleThree(Cherry::GetLocale("loc.windows.welcome.title"));

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
            std::string child_name;

            if (child.first.rfind("?loc:", 0) == 0)
            {
                std::string localeName = child.first.substr(5);
                child_name = Cherry::GetLocale(localeName) + "####" + localeName;
            }
            else
            {
                child_name = child.first;
            }

            if (Cherry::TextButtonUnderline(child_name.c_str()))
            {
                m_SelectedChildName = child.first;
            }

            ImGui::PopStyleColor();
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
