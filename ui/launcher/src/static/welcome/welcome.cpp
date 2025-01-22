#include "./welcome.hpp"

#include <cstdlib> // std::system
#include <string>
#include <cstring>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#include <shellapi.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

void OpenURL(const std::string &url)
{
#if defined(_WIN32)
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    std::string command = "open " + url;
    system(command.c_str());
#elif defined(__linux__)
    std::string command = "xdg-open " + url;
    system(command.c_str());
#else
    std::cerr << "Unsupported platform: unable to open URL." << std::endl;
#endif
}

bool ends_with(const std::string &value, const std::string &suffix)
{
    if (suffix.size() > value.size())
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

namespace VortexLauncher
{
    void Space(const float &space)
    {
        ImGui::BeginDisabled();
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::Button("", ImVec2(0, space));
        ImGui::PopStyleColor(2);
        ImGui::EndDisabled();
    }

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

    static void NewsBanner(bool disable_stack, const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &path = "resources/imgs/vortex_banner_unknow.png", const std::string &link_path = "", const std::string &desc = "")
    {
        ImVec2 squareSize(xsize, ysize);

        const char *originalText = envproject.c_str();
        char truncatedText[32];

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
            if (link_path != "")
            {
                OpenURL(link_path);
            }
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

        {
            ImVec2 textPadding(10.0f, 5.0f);
            float blurHeight = 30.0f;

            ImVec2 blurPos = ImVec2(cursorPos.x, cursorPos.y + squareSize.y - blurHeight);
            ImVec2 blurSize = ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y);

            drawList->AddRectFilled(blurPos, blurSize, IM_COL32(0, 0, 0, 128));

            ImVec2 textPos = ImVec2(cursorPos.x + textPadding.x, cursorPos.y + squareSize.y - blurHeight + textPadding.y);

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming font[1] is a larger font. Adjust index as needed.

            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), envproject.c_str());

            ImGui::PopFont();
        }

        ImVec2 textPos = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + 5);
        std::string fullText = desc;
        float maxWidth = squareSize.x;

        std::string line;
        float lineWidth = 0.0f;
        for (char ch : fullText)
        {
            ImVec2 charSize = ImGui::CalcTextSize(std::string(1, ch).c_str());
            if (lineWidth + charSize.x > maxWidth && !line.empty())
            {
                drawList->AddText(textPos, Cherry::HexToImU32("#999999ff"), line.c_str());
                textPos.y += charSize.y;
                line.clear();
                lineWidth = 0.0f;
            }
            line += ch;
            lineWidth += charSize.x;
        }

        if (!line.empty())
        {
            drawList->AddText(textPos, Cherry::HexToImU32("#999999ff"), line.c_str());
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


    static void VersionWelcomeMenu(bool disable_stack, const VortexVersion &envproject, int xsize = 100, int ysize = 100, const std::string &path = "resources/imgs/vortex_banner_unknow.png", bool clickable = true, std::function<void()> action = []() {})
    {
        ImVec2 squareSize(xsize, ysize);

        std::string vpath;
        bool version_installed = VortexMaker::CheckIfVortexVersionUtilityExist(envproject.version, vpath);

        const char *originalText = envproject.name.c_str();
        char truncatedText[32];

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

        std::string button_id = envproject.name + "squareButtonWithText" + envproject.name;
        if (!clickable)
        {
            ImGui::BeginDisabled();
        }
        if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
        {
            if (action)
            {
                action();
            }
        }
        if (!clickable)
        {
            ImGui::EndDisabled();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        ImDrawList *drawList = ImGui::GetWindowDrawList();

        if (!envproject.banner.empty() && std::filesystem::exists(envproject.banner))
        {
            drawList->AddImage(Cherry::GetTexture(envproject.banner), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y - 25.0f));
        }
        else
        {
            drawList->AddImage(Cherry::GetTexture(Cherry::GetPath(path)), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y - 25.0f));
        }

        if(version_installed)
        {
            drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("resources/imgs/installed_mask.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y - 25.0f));
        }
        else
        {
            drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("resources/imgs/install_mask.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y - 25.0f));
        }

        ImVec2 smallRectSize(40, 20);
        ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

        ImVec2 textPos(
            cursorPos.x + (squareSize.x - textSize.x) / 2.0f,
            cursorPos.y + squareSize.y + (squareSize.y - squareSize.y - textSize.y) / 2.0f - 5.0f);

        ImU32 textColor = Cherry::HexToImU32("#AEAEAEFF");
        drawList->AddText(ImVec2(textPos.x, textPos.y - 5.0f), textColor, truncatedText);

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

    static void QuickAction(bool disable_stack, const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &path = "resources/imgs/vortex_banner_unknow.png", bool clickable = true, std::function<void()> action = []() {})
    {
        ImVec2 squareSize(xsize, ysize);

        const char *originalText = envproject.c_str();
        char truncatedText[32];

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
        if (!clickable)
        {
            ImGui::BeginDisabled();
        }
        if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
        {
            if (action)
            {
                action();
            }
        }
        if (!clickable)
        {
            ImGui::EndDisabled();
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

    static void RecentProject(bool disable_stack, const std::shared_ptr<EnvProject> &envproject, int xsize = 100, int ysize = 100, const std::string &path = "resources/imgs/vortex_banner_unknow.png", bool clickable = true, std::function<void(const std::shared_ptr<EnvProject> &)> action = [](const std::shared_ptr<EnvProject> &) {})
    {
        ImVec2 squareSize(xsize, ysize);

        const char *originalText = envproject->name.c_str();
        char truncatedText[32];

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

        std::string button_id = envproject->path + "squareTdfgext####" + envproject->path;
        if (!clickable)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
        {
            std::cout << "Clicked" << std::endl;
            if (action)
            {
                action(envproject);
            }
        }

        if (!clickable)
        {
            ImGui::EndDisabled();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        ImDrawList *drawList = ImGui::GetWindowDrawList();

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            drawList->AddRect(cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y), IM_COL32(135, 135, 135, 255), 0.0f, 0, 2.0f);
        }

        ImU32 bgColor = Cherry::HexToImU32("#32323232");

        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y), bgColor, 0.0f);

        ImVec2 imageSize = ImVec2(75.0f, 75.0f);

        ImVec2 textPos(
            cursorPos.x + (squareSize.x - textSize.x) / 2.0f,
            cursorPos.y + imageSize.y + (squareSize.y - imageSize.y - textSize.y) / 2.0f + 10.0f);

        ImVec2 imagePos(
            cursorPos.x + 60,
            cursorPos.y + imageSize.y + (squareSize.y - imageSize.y - textSize.y) / 2.0f + 10.0f - 90.0f);

        drawList->AddImage(Cherry::GetTexture(path), imagePos, ImVec2(imagePos.x + imageSize.x, imagePos.y + imageSize.y));

        ImU32 textColor = Cherry::HexToImU32("#AEAEAEFF");
        drawList->AddText(ImVec2(textPos.x, textPos.y - 5.0f), textColor, truncatedText);

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
        };

        m_AppWindow->SetInternalPaddingX(0.0f);
        m_AppWindow->SetInternalPaddingY(0.0f);

        m_SelectedChildName = "?loc:loc.windows.welcome.overview";
        m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);

        this->AddChild("About", [this]()
                       { ImGui::Text("Latest 2 news + Changelog here + articles"); });

        this->AddChild("What's news ?", [this]()
                       { ImGui::Text("Latest 2 news + Changelog here + articles"); });

        this->AddChild("Learn", [this]() {

        });

        this->AddChild("?loc:loc.windows.welcome.overview", [this]()
                       {
                           float childWidth = ImGui::GetContentRegionAvail().x;
                           float inputTextWidth = 300.0f;
                           float buttonWidth = 100.0f;
                           float groupWidth = inputTextWidth + ImGui::GetStyle().ItemSpacing.x + buttonWidth;

                           // News
                           Cherry::TitleTwoColored("Overview", "#B1FF31FF");

                           Cherry::TitleSixColored("Latest news", "#797979FF");
                           if (VortexMaker::GetCurrentContext()->IO.offline)
                           {
                               NewsBanner(false, "Latest", 400, 150, "?", "resources/imgs/vortex_banner_disconnected.png");
                               NewsBanner(true, "All Versions", 400, 150, "", "resources/imgs/vortex_banner_disconnected.png");
                           }
                           else
                           {
                               for (const auto &article : VortexMaker::GetCurrentContext()->IO.news)
                               {
                                   if (!article.image_link.empty() &&
                                       (ends_with(article.image_link, ".png") || ends_with(article.image_link, ".jpg")) &&
                                       (article.image_link.find("http://") == 0 || article.image_link.find("https://") == 0))
                                   {
                                       NewsBanner(
                                           false,
                                           article.title,
                                           400, 150,
                                           Cherry::GetHttpPath(article.image_link),
                                           article.news_link,
                                           article.description);
                                   }
                               }
                           }

                           Space(200.0f);

                           Cherry::TitleSixColored("Fast actions", "#797979FF");
                           QuickAction(false, "create", 264, 120, Cherry::GetPath("resources/imgs/create_banner.png"), true, m_CreateProjectCallback);
                           QuickAction(false, "open", 263, 120, Cherry::GetPath("resources/imgs/open_banner.png"), true, m_OpenProjectCallback);
                           QuickAction(true, "settings", 264, 120, Cherry::GetPath("resources/imgs/settings_banner.png"), true, m_SettingsCallback);

                           for (auto project : VortexMaker::GetCurrentContext()->IO.sys_projects)
                           {
                           }

                           Cherry::TitleSixColored("Latest openned project", "#797979FF");
                           const size_t maxSlots = 4;

                           size_t filledSlots = 0;
for (const auto& project : m_RecentProjects) {
    bool isMostRecent = (filledSlots == 0);
    
    RecentProject(
        (filledSlots >= 3), 
        project,           
        196,         
        120,           
        project->logoPath,
            true, 
            [this](const std::shared_ptr<EnvProject>& p) { 
                m_ProjectCallback(p);
            }
    );

    ++filledSlots;
}


                           // Fill remaining slots with empty project placeholders
                           for (; filledSlots < maxSlots; ++filledSlots)
                           {
                               QuickAction((filledSlots >= 3 ? true : false), "", 196, 120, Cherry::GetPath("resources/imgs/empty_recent_project.png"), false);
                           }

                           Cherry::TitleSixColored("Latest available versions", "#797979FF");

                           int version_index = 0;
                           for (auto version : VortexMaker::GetCurrentContext()->latest_vortex_versions)
                           {
                               if (version_index < 3)
                               {
                                   VersionWelcomeMenu(false, version, 264, 120, Cherry::GetHttpPath(version.banner), true, [version]()
                                               { std::thread([version]()
                                                             { VortexMaker::OpenVortexInstaller(version.version, version.arch, version.dist, version.plat); })
                                                     .detach(); });
                                   version_index++;
                               }
                           }

                           while (version_index < 3)
                           {
                               QuickAction(false, "", 264, 120, Cherry::GetPath("resources/imgs/offline_vxv.png"), false);
                               version_index++;
                           } });

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
    }

    std::vector<std::shared_ptr<EnvProject>> WelcomeWindow::GetMostRecentProjects(const std::vector<std::shared_ptr<EnvProject>> &projects, size_t maxCount)
    {
        // Sort projects by lastOpened date in descending order
        auto sortedProjects = projects;
        std::sort(sortedProjects.begin(), sortedProjects.end(), [](const std::shared_ptr<EnvProject> &a, const std::shared_ptr<EnvProject> &b)
                  { return a->lastOpened > b->lastOpened; });

        // Return the top `maxCount` projects
        if (sortedProjects.size() > maxCount)
        {
            sortedProjects.resize(maxCount);
        }
        return sortedProjects;
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
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
        ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, NULL);

        ImGui::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/welcome.png")), ImVec2(270, 55));

        auto specialChild = std::find_if(m_Childs.begin(), m_Childs.end(), [](const auto &child)
                                         { return child.first == "?loc:loc.windows.welcome.overview"; });

        if (specialChild != m_Childs.end())
        {
            Cherry::MenuItemTextSeparator("Main");
            Cherry::TextButtonUnderlineOptions opt;

            if (specialChild->first == m_SelectedChildName)
            {
                opt.hex_text_idle = "#FFFFFFFF";
            }
            else
            {
                opt.hex_text_idle = "#A9A9A9FF";
            }
            std::string child_name;

            if (specialChild->first.rfind("?loc:", 0) == 0)
            {
                std::string localeName = specialChild->first.substr(5);
                child_name = Cherry::GetLocale(localeName) + "####" + localeName;
            }
            else
            {
                child_name = specialChild->first;
            }

            if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
            {
                m_SelectedChildName = specialChild->first;
            }
        }

        Cherry::MenuItemTextSeparator("All menus");
        for (const auto &child : m_Childs)
        {
            if (child.first == "?loc:loc.windows.welcome.overview")
                continue;

            Cherry::TextButtonUnderlineOptions opt;

            if (child.first == m_SelectedChildName)
            {
                opt.hex_text_idle = "#FFFFFFFF";
            }
            else
            {
                opt.hex_text_idle = "#A9A9A9FF";
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

            if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
            {
                m_SelectedChildName = child.first;
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();

        ImGui::SameLine();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#44444466"));
        ImGui::Button("splitter", ImVec2(splitterWidth, -1));
        ImGui::PopStyleColor();
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

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);

        if (!m_SelectedChildName.empty())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

            if (ImGui::BeginChild("ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar))
            {
                std::function<void()> pannel_render = GetChild(m_SelectedChildName);
                if (pannel_render)
                {
                    pannel_render();
                }
            }
            ImGui::EndChild();

            ImGui::PopStyleVar(2);
        }

        ImGui::EndGroup();
    }

}
