
#include "project_manager.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_set>

static std::vector<std::string> labels = {"Open a project", "Create a new project"};
static int selected = 0;
static int selected_template = 0;
static bool template_is_selected = false;
static bool open_import_projects = false;
static bool project_creation = false;
static std::shared_ptr<TemplateInterface> selected_template_object;
static std::shared_ptr<EnvProject> selected_envproject;
static std::shared_ptr<EnvProject> selected_envproject_to_remove;
static std::vector<std::shared_ptr<EnvProject>> finded_projects;
static std::string title = "none";
static std::string default_project_avatar = "/usr/local/include/Vortex/imgs/base_vortex.png";
static std::string operating_system_banner = "/usr/local/include/Vortex/1.1/imgs/operating_system_banner.png";
static std::string _parent;

static char ProjectSearch[256];
static float threshold = 0.4;

bool isOnlySpacesOrEmpty(const char *str)
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

std::string toLowerCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

int levenshteinDistance(const std::string &s1, const std::string &s2)
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

bool hasCommonLetters(const std::string &s1, const std::string &s2)
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

bool areStringsSimilar(const std::string &s1, const std::string &s2, double threshold)
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

ProjectManager::ProjectManager()
{
    m_AppWindow = std::make_shared<Cherry::AppWindow>("?loc:loc.window_names.project_manager", "?loc:loc.window_names.project_manager");
    m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
    m_AppWindow->SetDefaultBehavior(Cherry::DefaultAppWindowBehaviors::DefaultDocking, "full");
    m_AppWindow->SetClosable(false);

    m_AppWindow->SetLeftMenubarCallback([this]()
                                        { this->mainButtonsMenuItem(); });

    this->ctx = VortexMaker::GetCurrentContext();
    this->Refresh();
}

void MyButton(const std::string &name, int w, int h)
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
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/misc/icon_vortex_default.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
    }

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

// Fonction pour dessiner du texte avec des correspondances mises en surbrillance
void DrawHighlightedText(ImDrawList *drawList, ImVec2 textPos, const char *text, const char *search, ImU32 highlightColor, ImU32 textColor, ImU32 highlightTextColor)
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

void MyButton(const std::shared_ptr<EnvProject> envproject)
{
    ImVec2 squareSize(100, 100);

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
        drawList->AddImage(Cherry::GetTexture(Cherry::GetPath("ressources/imgs/icons/misc/icon_vortex_default.png")), cursorPos, ImVec2(cursorPos.x + squareSize.x, cursorPos.y + squareSize.y));
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

void MyButton(const std::shared_ptr<TemplateInterface> templateinterface)
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

void MyBanner(const std::string &path)
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

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);
}

void ProjectManager::RefreshRender(const std::shared_ptr<ProjectManager> &instance)
{
    m_AppWindow->SetRenderCallback([instance]()
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
                                           static ImGuiTableFlags window_flags = ImGuiWindowFlags_MenuBar;
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
                                                   open_import_projects = false;
                                                   ImGui::CloseCurrentPopup();
                                               }
                                               ImGui::EndPopup();
                                           }
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
                                           ImGui::OpenPopup("Search for project(s) to add");

                                       if (!project_creation)
                                       {
                                           ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

                                            // TODO Project filter (groups)

                                            Cherry::TitleFourColored("All projects ", "#75757575");
                                           for (int row = 0; row < instance->ctx->IO.sys_projects.size(); row++)
                                           {
                                               if (areStringsSimilar(instance->ctx->IO.sys_projects[row]->name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                                               {
                                                   MyButton(instance->ctx->IO.sys_projects[row]);
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

                                           ImGui::BeginChild("###rightpan");
                                           if (!selected_envproject)
                                           {
                                               ImGui::Text("Select a project");
                                           }
                                           else
                                           {
                                               {
                                                   ImGui::BeginChild("LOGO_", ImVec2(70, 70), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                                                       MyButton(selected_envproject->logoPath, 60, 60);
                                                   ImGui::EndChild();
                                                   ImGui::SameLine();
                                               }

                                               {
                                                   ImGuiID _id = ImGui::GetID("TEXT_");
                                                   ImGui::BeginChild(_id, ImVec2(0, 70), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                                                   float oldsize = ImGui::GetFont()->Scale;
                                                   ImGui::GetFont()->Scale *= 1.3;
                                                   ImGui::PushFont(ImGui::GetFont());

                                                   ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_envproject->name.c_str());

                                                   ImGui::GetFont()->Scale = oldsize;
                                                   ImGui::PopFont();

                                                   ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Last opened ");
                                                   ImGui::SameLine();
                                                   ImGui::Text(selected_envproject->lastOpened.c_str());

                                                   ImGui::EndChild();
                                               }

                                               ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) ");
                                               ImGui::SameLine();
                                               ImGui::Text(selected_envproject->author.c_str());

                                               ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description ");
                                               ImGui::SameLine();
                                               ImGui::Text(selected_envproject->description.c_str());

                                               ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Version ");
                                               ImGui::SameLine();
                                               ImGui::Text(selected_envproject->version.c_str());

                                               ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Compatible with ");
                                               ImGui::SameLine();
                                               ImGui::Text(selected_envproject->version.c_str());

                                               ImVec2 windowSize = ImGui::GetWindowSize();
                                               ImVec2 contentSize = ImGui::GetContentRegionAvail();
                                               ImVec2 buttonSize = ImVec2(100, 30);

                                               float firstButtonPosX = windowSize.x - buttonSize.x * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

                                               float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - 5;

                                               ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

                                               if (ImGui::Button("Delete", buttonSize))
                                               {
                                                   selected_envproject_to_remove = selected_envproject;
                                                   open_deletion_modal = true;
                                               }

                                               ImGui::SameLine();

                                               ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 1.0f, 0.8f));
                                               ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 1.0f, 1.0f));
                                               ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.0f, 1.8f));
                                               if (ImGui::Button("Open Project", buttonSize))
                                               {

                                                   std::string projectPath = selected_envproject->path;

                                                   std::thread([projectPath]()
                                                               {
        std::string cmd = "cd " + projectPath + " && vortex -e";
        system(cmd.c_str()); })
                                                       .detach();
                                               }
                                               ImGui::PopStyleColor(3);
                                           }

                                           ImGui::EndChild();
                                       }
                                       else
                                       {
                                           float columnWidths[3] = {100.0f, 250.0f, 150.0f};

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
                                                   MyBanner("/usr/local/include/Vortex/1.1/imgs/b_all_bases.png");
                                                   MyBanner("/usr/local/include/Vortex/1.1/imgs/b_operating_systems.png");
                                                   MyBanner("/usr/local/include/Vortex/1.1/imgs/b_app_svc.png");
                                                   MyBanner("/usr/local/include/Vortex/1.1/imgs/b_tools_utils.png");
                                                   MyBanner("/usr/local/include/Vortex/1.1/imgs/b_assets_components.png");
                                               }

                                               else if (i == 1)
                                               {
                                                   for (int project = 0; project < instance->project_templates.size(); project++)
                                                   {
                                                       if (instance->project_templates[project] != NULL)
                                                       {
                                                           MyButton(instance->project_templates[project]);
                                                       }
                                                   }
                                               }
                                               else if (i == 2)
                                               {
                                                   if (!selected_template_object)
                                                   {
                                                       ImGui::Text("Select a template");
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

                                                           ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), name);
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
                                                       ImGui::InputText("Path", buf, sizeof(buf));
                                                       ImGui::InputText("Name", name, 128);
                                                       ImGui::InputText("Description", desc, 128);
                                                       ImGui::InputText("Authors", auth, 128);
                                                       ImGui::InputText("Version", version, 128);

                                                       ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                                                       ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                                                       ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                                                       ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                                                       float ysection = windowSize.y - 280;
                                                       ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

                                                       ImGui::Text("Configuration of the project :");
                                                       ImGui::Checkbox("Open the project after creation", &open);
                                                       std::string path = s + name;

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
                                                           VortexMaker::CreateProject(name, auth, version, desc, path, path + "/icon.png", selected_template_object->m_name);
                                                           VortexMaker::RefreshEnvironmentProjects();

                                                           if (open)
                                                           {
                                                               std::string projectPath = path;

                                                               std::thread([projectPath]()
                                                                           {
        std::string cmd = "cd " + projectPath + " && vortex -e";
        system(cmd.c_str()); })
                                                                   .detach();
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
    create_project_button->SetImagePath(Cherry::GetPath("ressources/imgs/icons/misc/icon_add.png"));

    static std::shared_ptr<Cherry::ImageTextButtonSimple> import_project_button = std::make_shared<Cherry::ImageTextButtonSimple>("import_btn", "Import a project");
    import_project_button->SetScale(0.85f);
    import_project_button->SetInternalMarginX(10.0f);
    import_project_button->SetLogoSize(15, 15);
    import_project_button->SetImagePath(Cherry::GetPath("ressources/imgs/icons/misc/icon_import.png"));

    static std::shared_ptr<Cherry::ImageTextButtonSimple> open_project_button = std::make_shared<Cherry::ImageTextButtonSimple>("open_btn", "Open a project");
    open_project_button->SetScale(0.85f);
    open_project_button->SetInternalMarginX(10.0f);
    open_project_button->SetLogoSize(15, 15);
    create_project_button->SetImagePath(Cherry::GetPath("ressources/imgs/icon.png"));

    static std::shared_ptr<std::string> v_SearchString = std::make_shared<std::string>("");
    static std::shared_ptr<Cherry::ImageStringInput> input_search = std::make_shared<Cherry::ImageStringInput>("open_btn", v_SearchString, Cherry::GetPath("ressources/imgs/icon.png"), "Open a project");

    if (!project_creation)
    {
        if (create_project_button->Render())
        {
            project_creation = true;
        }

        if (import_project_button->Render())
        {
            open_import_projects = true;
        }

        ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#45454545")),
            ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::PushItemWidth(200);
        input_search->Render("_");
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