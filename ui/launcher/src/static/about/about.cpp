#include "./about.hpp"

namespace VortexLauncher {
  AboutAppWindow::AboutAppWindow(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    m_AppWindow->SetInternalPaddingX(8.0f);
    m_AppWindow->SetInternalPaddingY(8.0f);

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::shared_ptr<Cherry::AppWindow> &AboutAppWindow::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<AboutAppWindow> AboutAppWindow::Create(const std::string &name) {
    auto instance = std::shared_ptr<AboutAppWindow>(new AboutAppWindow(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void AboutAppWindow::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  void AboutAppWindow::Render() {
    float window_width = ImGui::GetWindowSize().x;
    float image_height = window_width / 3.435f;

    ImGui::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortexbanner2.png")), ImVec2(window_width, image_height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));  // CherryStyle::Padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));   // CherryStyle::Padding
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
    ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryStyle::AddMarginX(10.0f);
    ImGui::BeginChild("aboutchild");  // cherry api

    CherryKit::TitleFive("Vortex Launcher");

    std::string version = VORTEXLAUNCHER_VERSION;
    Cherry::PushPermanentProperty("color_text", "#797979");
    CherryKit::TextSimple("Version: " + version);
    CherryKit::TextSimple("Hash: QSFG5121");
    CherryKit::TextSimple("System: Arch Linux x86_64 - Wayland");

    CherryKit::Space(12.0f);

    CherryKit::TextSimple("Vortex is a open and free software.");
    CherryKit::TextSimple("Licensed under the  Apache-2.0 license ");
    CherryKit::Space(12.0f);
    Cherry::PopPermanentProperty();

    CherryKit::TextSimple("Never stop hacking !");

    ImGui::EndChild();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);  // CherryStyle::Padding
  }

}  // namespace VortexLauncher
