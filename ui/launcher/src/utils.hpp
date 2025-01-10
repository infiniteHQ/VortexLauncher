#pragma once
#include "../../../main/include/vortex.h"
#include "../../../main/include/vortex_internals.h"

#define CHERRY_V1
#include "../../../lib/cherry/cherry.hpp"

#ifndef UTILS_CHERRY_LAUNCHER
#define UTILS_CHERRY_LAUNCHER

static std::vector<std::string> labels = {"Open a project", "Create a new project"};
static int selected = 0;
static int selected_template = 0;
static bool template_is_selected = false;
static bool open_import_projects = false;
static bool showProjectPools = false;
static std::shared_ptr<TemplateInterface> selected_template_object;
static std::shared_ptr<EnvProject> selected_envproject;
static std::shared_ptr<EnvProject> selected_envproject_to_remove;
static std::string title = "none";
static std::string default_project_avatar = "/usr/local/include/Vortex/imgs/base_vortex.png";
static std::string operating_system_banner = "/usr/local/include/Vortex/1.1/imgs/operating_system_banner.png";
static std::string _parent;

static std::vector<std::shared_ptr<EnvProject>> finded_projects;
static std::vector<std::shared_ptr<EnvProject>> finded_projects_to_import;

static char ProjectSearch[256];
static float threshold = 0.4;

static void OpenFolderInFileManager(std::string path)
{
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "explorer \"" + path + "\"";
    std::system(command.c_str());
#elif defined(__APPLE__)
    std::string command = "open \"" + path + "\"";
    std::system(command.c_str());
#elif defined(__linux__)
    std::string command =
        "dbus-send --session "
        "--dest=org.freedesktop.FileManager1 "
        "--type=method_call "
        "/org/freedesktop/FileManager1 "
        "org.freedesktop.FileManager1.ShowFolders "
        "array:string:\"" +
        path + "\" string:\"\"";

    int retCode = std::system(command.c_str());
    if (retCode != 0)
    {
        std::cerr << "D-Bus method failed, trying xdg-open...\n";
        command = "xdg-open \"" + path + "\"";
        std::system(command.c_str());
    }
#else
#error "OS not supported!"
#endif
}
static void saveVortexVersions(const std::vector<std::string> &paths, const std::string &jsonFilePath)
{
    nlohmann::json jsonData;
    jsonData["vortex_versions_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file)
    {
        file << jsonData.dump(4);
    }
}

static void loadVortexVersions(std::vector<std::string> &paths, const std::string &jsonFilePath)
{
    std::ifstream file(jsonFilePath);
    if (file)
    {
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto &path : jsonData["vortex_versions_pools"])
        {
            paths.push_back(path.get<std::string>());
        }
    }
}

static void saveTemplates(const std::vector<std::string> &paths, const std::string &jsonFilePath)
{
    nlohmann::json jsonData;
    jsonData["templates_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file)
    {
        file << jsonData.dump(4);
    }
}

static void loadTemplates(std::vector<std::string> &paths, const std::string &jsonFilePath)
{
    std::ifstream file(jsonFilePath);
    if (file)
    {
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto &path : jsonData["templates_pools"])
        {
            paths.push_back(path.get<std::string>());
        }
    }
}

static void saveModules(const std::vector<std::string> &paths, const std::string &jsonFilePath)
{
    nlohmann::json jsonData;
    jsonData["modules_pools"] = paths;

    std::ofstream file(jsonFilePath);
    if (file)
    {
        file << jsonData.dump(4);
    }
}

static void loadModules(std::vector<std::string> &paths, const std::string &jsonFilePath)
{
    std::ifstream file(jsonFilePath);
    if (file)
    {
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto &path : jsonData["modules_pools"])
        {
            paths.push_back(path.get<std::string>());
        }
    }
}

static void saveProjects(const std::vector<std::string> &projectPaths, const std::string &jsonFilePath)
{
    nlohmann::json jsonData;
    jsonData["projects_pools"] = projectPaths;

    std::ofstream file(jsonFilePath);
    if (file)
    {
        file << jsonData.dump(4);
    }
}

static void loadProjects(std::vector<std::string> &projectPaths, const std::string &jsonFilePath)
{
    std::ifstream file(jsonFilePath);
    if (file)
    {
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto &path : jsonData["projects_pools"])
        {
            projectPaths.push_back(path.get<std::string>());
        }
    }
}

static bool isOnlySpacesOrEmpty(const char *str)
{
    if (str == nullptr || std::strlen(str) == 0)
    {
        return true;
    }

    for (size_t i = 0; i < std::strlen(str); ++i)
    {
        if (str[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

static std::string toLowerCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static int levenshteinDistance(const std::string &s1, const std::string &s2)
{
    const size_t m = s1.size();
    const size_t n = s2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (size_t i = 0; i <= m; ++i)
    {
        for (size_t j = 0; j <= n; ++j)
        {
            if (i == 0)
            {
                dp[i][j] = j;
            }
            else if (j == 0)
            {
                dp[i][j] = i;
            }
            else
            {
                int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
            }
        }
    }
    return dp[m][n];
}

static bool hasCommonLetters(const std::string &s1, const std::string &s2)
{
    std::unordered_set<char> set1(s1.begin(), s1.end());
    for (char c : s2)
    {
        if (set1.find(c) != set1.end())
        {
            return true;
        }
    }
    return false;
}

static bool areStringsSimilar(const std::string &s1, const std::string &s2, double threshold)
{
    std::string lower_s1 = toLowerCase(s1);
    std::string lower_s2 = toLowerCase(s2);

    int dist = levenshteinDistance(lower_s1, lower_s2);
    int maxLength = std::max(lower_s1.size(), lower_s2.size());
    double similarity = 1.0 - (static_cast<double>(dist) / maxLength);

    if (std::strlen(ProjectSearch) < 5)
    {
        return similarity >= threshold || hasCommonLetters(lower_s1, lower_s2);
    }

    return similarity >= threshold;
}

static void MyButton(const std::string &name, int w, int h)
{
    ImVec2 squareSize(w, h);
    ImVec2 totalSize(squareSize.x, squareSize.y + 5);
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    std::string button_id = name + "squareButtonWithText";
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    if (!name.empty())
    {
        drawList->AddImage(Cherry::GetTexture(name), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }
    else
    {
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_vortex_default.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

static void DrawHighlightedText(ImDrawList *drawList, ImVec2 textPos, const char *text, const char *search, ImU32 highlightColor, ImU32 textColor, ImU32 highlightTextColor)
{
    if (!text || !search || !*search)
    {
        drawList->AddText(textPos, textColor, text);
        return;
    }

    const char *start = text;
    const char *found = strstr(start, search);
    while (found)
    {
        // Dessiner le texte avant la correspondance
        if (found > start)
        {
            std::string preText(start, found);
            drawList->AddText(textPos, textColor, preText.c_str());
            textPos.x += ImGui::CalcTextSize(preText.c_str()).x;
        }

        // Dessiner la correspondance mise en surbrillance avec un texte noir
        ImVec2 highlightPos = textPos;
        ImVec2 highlightSize = ImGui::CalcTextSize(search);
        drawList->AddRectFilled(highlightPos, ImVec2(highlightPos.x + highlightSize.x, highlightPos.y + highlightSize.y), highlightColor);
        drawList->AddText(textPos, highlightTextColor, search);
        textPos.x += highlightSize.x;

        // Passer à la partie suivante du texte
        start = found + strlen(search);
        found = strstr(start, search);
    }

    // Dessiner le texte restant après la dernière correspondance
    if (*start)
    {
        drawList->AddText(textPos, textColor, start);
    }
}

static void VersionButton(const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &version = "?", const std::string &path = "resources/imgs/vortex_banner_unknow.png")
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

    DrawHighlightedText(drawList, textPos, truncatedText, ProjectSearch, highlightColor, textColor, highlightTextColor);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

static void DownloadableVersionButton(const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &version = "?", const std::string &path = "resources/imgs/vortex_banner_unknow.png", std::string installedpath = "none")
{
    ImVec2 squareSize(xsize, ysize);
    const char *originalText = envproject.c_str();
    char truncatedText[32];
    const char *versionText = version.c_str();
    bool exist = VortexMaker::CheckIfVortexVersionUtilityExist(version, installedpath);

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

    float spacingX = 15.0f;
    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    ImGui::BeginGroup();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList *drawList = ImGui::GetWindowDrawList();

    if (cursorPos.x + totalSize.x > windowVisibleX2)
    {
        ImGui::NewLine();
        cursorPos = ImGui::GetCursorScreenPos();
    }

    if (!envproject.empty() && std::filesystem::exists(envproject))
    {
        drawList->AddImage(Cherry::GetTexture(envproject), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }
    else
    {
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath(path)), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos;
    if (exist)
    {
        smallRectPos = ImVec2(cursorPos.x + squareSize.x - smallRectSize.x - 160, cursorPos.y + squareSize.y - smallRectSize.y - 5);
    }
    else
    {
        smallRectPos = ImVec2(cursorPos.x + squareSize.x - smallRectSize.x - 40, cursorPos.y + squareSize.y - smallRectSize.y - 5);
    }
    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));

    ImVec2 versionTextPos = ImVec2(smallRectPos.x + 6, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize(versionText).y) / 2);
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);

    ImVec2 dotButtonPos = ImVec2(smallRectPos.x + smallRectSize.x + 15, smallRectPos.y);
    ImGui::SetCursorScreenPos(dotButtonPos);

    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string dist = ctx->IO.sys_vortexlauncher_dist;
    if (exist)
    {
        {
            ImGui::BeginDisabled();
            auto btn = std::make_shared<Cherry::ImageTextButtonSimple>("create_project_button", "Already installed");
            btn->SetScale(0.85f);
            btn->SetInternalMarginX(10.0f);
            btn->SetInternalMarginY(4.0f);
            btn->SetLogoSize(1, 1);
            btn->SetBorderColorIdle("#00000000");
            btn->SetBackgroundColorClicked("#00000000");
            btn->SetImagePath(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));

            if (btn->Render(envproject))
            {
                //
            }
            ImGui::EndDisabled();
        }
        ImGui::SameLine();

        {
            auto btn = std::make_shared<Cherry::ImageButtonSimple>("create_project_button", Cherry::GetPath("resources/imgs/icons/misc/icon_more.png"));
            btn->SetScale(0.20f);
            btn->SetInternalMarginX(1.0f);
            btn->SetInternalMarginY(1.0f);
            btn->SetLogoSize(5, 5);
            btn->SetBorderColorIdle("#00000000");
            btn->SetBackgroundColorClicked("#00000000");

            if (btn->Render(envproject + "_menu"))
            {
                ImGui::OpenPopup(("OptionsMenu_" + envproject).c_str());
            }

            // Menu contextuel
            if (ImGui::BeginPopup(("OptionsMenu_" + envproject).c_str()))
            {
                ImVec4 originalColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                if (ImGui::MenuItem("Reinstall"))
                {
                    std::thread([version, ctx, dist, installedpath]()
                                { 
            VortexMaker::OpenVortexUninstaller(installedpath);
            VortexMaker::OpenVortexInstaller(version, ctx->arch, dist, ctx->platform); })
                        .detach();
                }
                ImGui::PopStyleColor();
                if (ImGui::MenuItem("Open Folder###Open1"))
                {
                    OpenFolderInFileManager(installedpath);
                }

                ImGui::EndPopup();
            }
        }
    }
    else
    {
        std::string label = "install_vortex_version" + envproject;
        auto btn = std::make_shared<Cherry::ImageButtonSimple>(label, Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"));
        btn->SetScale(0.20f);
        btn->SetInternalMarginX(1.0f);
        btn->SetInternalMarginY(1.0f);
        btn->SetLogoSize(5, 5);
        btn->SetBorderColorIdle("#00000000");
        btn->SetBackgroundColorClicked("#00000000");

        if (btn->Render(envproject))
        {
            std::thread([version, ctx, dist]()
                        { VortexMaker::OpenVortexInstaller(version, ctx->arch, dist, ctx->platform); })
                .detach();
        }
    }

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), truncatedText);
    drawList->AddText(ImVec2(textPos.x, textPos.y - 20), IM_COL32(255, 255, 255, 255), dist.c_str());

    ImGui::EndGroup();

    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + totalSize.x + spacingX, cursorPos.y));

    if (cursorPos.x + totalSize.x + spacingX > windowVisibleX2)
    {
        ImGui::NewLine();
    }
}

static void InstalledVersionButton(const std::string &path, const std::string &envproject, int xsize = 100, int ysize = 100, const std::string &version = "?", const std::string &bannerpath = "resources/imgs/vortex_banner_unknow.png")
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

    float spacingX = 15.0f;
    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    ImGui::BeginGroup();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList *drawList = ImGui::GetWindowDrawList();

    if (cursorPos.x + totalSize.x > windowVisibleX2)
    {
        ImGui::NewLine();
        cursorPos = ImGui::GetCursorScreenPos();
    }

    if (!envproject.empty() && std::filesystem::exists(envproject))
    {
        drawList->AddImage(Cherry::GetTexture(envproject), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }
    else
    {
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath(bannerpath)), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 40, cursorPos.y + squareSize.y - smallRectSize.y - 5);
    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));

    ImVec2 versionTextPos = ImVec2(smallRectPos.x + 6, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize(versionText).y) / 2);
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);

    ImVec2 dotButtonPos = ImVec2(smallRectPos.x + smallRectSize.x + 15, smallRectPos.y);
    ImGui::SetCursorScreenPos(dotButtonPos);

    auto btn = std::make_shared<Cherry::ImageButtonSimple>("create_project_button", Cherry::GetPath("resources/imgs/icons/misc/icon_more.png"));
    btn->SetScale(0.20f);
    btn->SetInternalMarginX(1.0f);
    btn->SetInternalMarginY(1.0f);
    btn->SetLogoSize(5, 5);
    btn->SetBorderColorIdle("#00000000");
    btn->SetBackgroundColorClicked("#00000000");

    if (btn->Render(bannerpath))
    {
        ImGui::OpenPopup(("OptionsMenu_" + bannerpath).c_str());
    }

    if (ImGui::BeginPopup(("OptionsMenu_" + bannerpath).c_str()))
    {
        ImVec4 originalColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::MenuItem("Delete Version"))
        {
            VortexMaker::OpenVortexUninstaller(path);
        }
        ImGui::PopStyleColor();
        if (ImGui::MenuItem("Open Folder###Open1"))
        {
            OpenFolderInFileManager(path);
        }
        ImGui::EndPopup();
    }

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), truncatedText);

    ImGui::EndGroup();

    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + totalSize.x + spacingX, cursorPos.y));

    if (cursorPos.x + totalSize.x + spacingX > windowVisibleX2)
    {
        ImGui::NewLine();
    }
}

static void MyButton(const std::shared_ptr<EnvProject> envproject, int xsize = 100, int ysize = 100)
{
    ImVec2 squareSize(xsize, ysize);

    const char *originalText = envproject->name.c_str();
    char truncatedText[12];
    const char *versionText = envproject->compatibleWith.c_str();

    if (strlen(originalText) > 8)
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

    std::string button_id = envproject->name + "squareButtonWithText" + envproject->lastOpened;
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
        selected_envproject = envproject;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    if (!envproject->logoPath.empty())
    {
        drawList->AddImage(Cherry::GetTexture(envproject->logoPath), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }
    else
    {
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_vortex_default.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
    ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(versionText).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);

    std::string versionpath;

    if (VortexMaker::CheckIfVortexVersionUtilityExist(envproject->compatibleWith, versionpath))
    {
        drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);
    }
    else
    {
        drawList->AddText(versionTextPos, IM_COL32(255, 20, 20, 255), versionText);
    }

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);

    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y), IM_COL32(135, 135, 135, 255), 0.0f, 0, 2.0f);
    }

    DrawHighlightedText(drawList, textPos, truncatedText, ProjectSearch, highlightColor, textColor, highlightTextColor);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

static void ProjectImportButton(const std::shared_ptr<EnvProject> envproject, int xsize = 100, int ysize = 100)
{
    ImVec2 squareSize(xsize, ysize);

    const char *originalText = envproject->name.c_str();
    char truncatedText[12];
    const char *versionText = envproject->compatibleWith.c_str();

    if (strlen(originalText) > 8)
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

    std::string button_id = envproject->name + "squareButtonWithText" + envproject->lastOpened;
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
        finded_projects_to_import.push_back(envproject);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    if (!envproject->logoPath.empty())
    {
        drawList->AddImage(Cherry::GetTexture(envproject->logoPath), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }
    else
    {
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_vortex_default.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
    ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(versionText).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);

    std::string versionpath;

    if (VortexMaker::CheckIfVortexVersionUtilityExist(envproject->compatibleWith, versionpath))
    {
        drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), versionText);
    }
    else
    {
        drawList->AddText(versionTextPos, IM_COL32(255, 20, 20, 255), versionText);
    }

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);

    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y), IM_COL32(135, 135, 135, 255), 0.0f, 0, 2.0f);
    }

    DrawHighlightedText(drawList, textPos, truncatedText, ProjectSearch, highlightColor, textColor, highlightTextColor);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

static void MyButton(const std::shared_ptr<TemplateInterface> templateinterface)
{
    ImVec2 squareSize(100, 100);

    const char *originalText = templateinterface->m_proper_name.c_str();
    char truncatedText[12];

    if (strlen(originalText) > 8)
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

    std::string button_id = templateinterface->m_name + "squareButtonWithText";
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
        selected_template_object = templateinterface;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    std::string logo_path = templateinterface->m_logo_path;

    drawList->AddImage(Cherry::GetTexture(templateinterface->m_logo_path), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
    ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(templateinterface->m_group.c_str()).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), templateinterface->m_group.c_str());

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), truncatedText);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

static void MyBanner(const std::string &path)
{
    // addTexture(path, path);
    ImVec2 squareSize(300, 70);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    if (ImGui::InvisibleButton("##squ5684areButtonWithText2", squareSize))
    {
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    // getTexture(path, drawList, cursorPos, squareSize);
    // ImGui::Image(path, drawList, cursorPos, squareSize);
    drawList->AddImage(Cherry::GetTexture(path), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);
}

#endif // UTILS_CHERRY_LAUNCHER