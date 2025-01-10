#include "./about.hpp"

namespace VortexLauncher
{
    AboutAppWindow::AboutAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

        m_AppWindow->SetInternalPaddingX(8.0f);
        m_AppWindow->SetInternalPaddingY(8.0f);

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
    }

    std::shared_ptr<Cherry::AppWindow> &AboutAppWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<AboutAppWindow> AboutAppWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<AboutAppWindow>(new AboutAppWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void AboutAppWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void AboutAppWindow::Render()
    {

        // Obtenir la largeur de la fenêtre
        float windowWidth = ImGui::GetWindowSize().x;

        // Centrer le titre
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Vortex").x) * 0.5f - 21.0f);
        Cherry::TitleOne("Vortex");

        // Centrer le sous-titre
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Creation Platform").x) * 0.5f);
        ImGui::Text("Creation Platform");

        // Centrer la version
        {
            std::string version = VORTEX_VERSION;
            std::string label = "Launcher v. " + version;

            ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(label.c_str()).x) * 0.5f);
            ImGui::Text(label.c_str());
        }

        ImGui::BeginDisabled();
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::Button("", ImVec2(0, 8.0f));
        ImGui::PopStyleColor(2);
        ImGui::EndDisabled();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(16.0f, 0.0f)); // Ajustez la valeur de X pour contrôler le padding horizontal

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Summary"))
            {
                float space = 5.0f;
                ImGui::TextWrapped("Vortex is a free, open, and open-source platform designed to provide ethical tools to creators, enabling them to create, modify, repair, or hack all kinds of projects. It offers powerful tools while paying close attention to fostering values of ethics, humanity, and environmental responsibility.");

                ImGui::BeginDisabled();
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
                ImGui::Button("", ImVec2(0, space));
                ImGui::PopStyleColor(2);
                ImGui::EndDisabled();
                ImGui::TextWrapped("Vortex is a project that includes an editor for creating projects, diagnostic or utility tools, and a community of creators, hackers, and users who aspire to build sovereign computing, achieve independence, and benefit from technology that is healthy for the planet and aligned with the interests of everyone.");

                ImGui::BeginDisabled();
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
                ImGui::Button("", ImVec2(0, space));
                ImGui::PopStyleColor(2);
                ImGui::EndDisabled();
                ImGui::TextWrapped("Vortex is also an active creative community where many people contribute daily to building all kinds of things, both hardware and software, and share them with everyone to foster public and impactful innovation. We believe that open-source innovation is the most powerful!");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Credits"))
            {
        ImGui::Text("Original author : Diego Moreno");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    ImGui::PopStyleVar(); // Restaurer le style
    }

}
