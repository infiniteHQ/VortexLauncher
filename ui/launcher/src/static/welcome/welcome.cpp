#include "./welcome.hpp"

namespace Cherry
{
    WelcomeWindow::WelcomeWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
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

        m_SelectedChildName = "?loc:loc.window_names.welcome.overview";

        this->AddChild("?loc:loc.window_names.welcome.sponsor", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.window_names.welcome.contribute", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.window_names.welcome.fast_actions", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.window_names.welcome.trends", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.window_names.welcome.actuality", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.window_names.welcome.whats_news", [this]()
                       {
                           //
                       });
        this->AddChild("?loc:loc.window_names.welcome.overview", [this]()
                       {
                           ImGui::Text("Overview");
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

    std::function<void()> WelcomeWindow::GetChild(const std::string &child_name)
    {
        auto it = m_Childs.find(child_name);
        if (it != m_Childs.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void WelcomeWindow::RefreshRender(const std::shared_ptr<WelcomeWindow> &instance)
    {
        m_AppWindow->SetRenderCallback([instance]()
                                       {
                                           static float leftPaneWidth = 300.0f;
                                           const float minPaneWidth = 50.0f;
                                           const float splitterWidth = 1.5f;
                                           static int selected;


                                           ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);


            TitleThree(Cherry::GetLocale("loc.window_names.welcome.title"));

        for (const auto &child : instance->m_Childs)                                           
        {
                                               if (child.first == instance->m_SelectedChildName)
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
            child_name = Application::Get().GetLocale(localeName) + "####" + localeName;
        }
        else
        {
            child_name = child.first;
        }


                                               if (TextButtonUnderline(child_name.c_str()))
                                               {
                                                   instance->m_SelectedChildName = child.first;
                                               }

                                               ImGui::PopStyleColor();
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
