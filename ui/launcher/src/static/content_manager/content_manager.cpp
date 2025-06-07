#include "./content_manager.hpp"

#include <cstdlib>  // std::system
#include <cstring>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

namespace VortexLauncher {

  static bool delete_content_modal_opened = false;
  static bool import_content_modal_opened = false;
  static std::shared_ptr<ContentInterface> content_to_delete = nullptr;
  bool ContentManager::ItemContentCard(
      const std::string &name,
      const std::string &path,
      const std::string &description,
      const std::string &size,
      bool selected,
      const std::string &logo,
      ImU32 bgColor,
      ImU32 borderColor,
      ImU32 lineColor,
      float maxTextWidth,
      float borderRadius,
      const std::shared_ptr<ContentInterface> &content) {
    bool pressed = false;

    float logoSize = 60.0f;
    float extraHeight = 80.0f;
    float padding = 10.0f;
    float separatorHeight = 2.0f;
    float textOffsetY = 5.0f;
    float versionBoxWidth = 10.0f;
    float versionBoxHeight = 20.0f;
    float thumbnailIconOffsetY = 30.0f;

    float oldfontsize = CherryGUI::GetFont()->Scale;
    ImFont *oldFont = CherryGUI::GetFont();

    if (selected) {
      bgColor = IM_COL32(80, 80, 240, 255);
      borderColor = IM_COL32(150, 150, 255, 255);
    }

    ImVec2 squareSize(logoSize, logoSize);

    const char *originalText = name.c_str();
    std::string truncatedText = name;

    if (CherryGUI::CalcTextSize(originalText).x > maxTextWidth) {
      truncatedText = name.substr(0, 20);
      if (CherryGUI::CalcTextSize(truncatedText.c_str()).x > maxTextWidth) {
        truncatedText = name.substr(0, 10) + "\n" + name.substr(10, 10);
      }
    } else {
      truncatedText = name + "\n";
    }

    ImVec2 fixedSize(maxTextWidth + padding * 2, logoSize + extraHeight + padding * 2);

    ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

    std::string button_id = name + "squareButtonWithText" + name;
    if (CherryGUI::InvisibleButton(button_id.c_str(), fixedSize)) {
      pressed = true;
    }

    if (CherryGUI::IsItemHovered()) {
      CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);

    ImDrawList *drawList = CherryGUI::GetWindowDrawList();

    drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), bgColor, borderRadius);
    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y),
        IM_COL32(26, 26, 26, 255),
        borderRadius,
        ImDrawFlags_RoundCornersTop);
    drawList->AddRect(
        cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), borderColor, borderRadius, 0, 1.0f);

    ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + padding);

    ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 20 + textOffsetY);
    CherryGUI::SetCursorScreenPos(sizePos);

    static ImTextureID logotexture = Application::GetCurrentRenderedWindow()->get_texture(logo);
    drawList->AddImage(logotexture, logoPos, ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

    CherryGUI::GetFont()->Scale = 0.7;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(maxTextWidth);
    CherryGUI::TextWrapped(size.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
    ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
    drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

    CherryGUI::GetFont()->Scale = 0.9;
    CherryGUI::PushFont(CherryGUI::GetFont());

    ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
    CherryGUI::SetCursorScreenPos(textPos);
    CherryGUI::PushItemWidth(maxTextWidth);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

    CherryGUI::PopItemWidth();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 descriptionPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
    CherryGUI::SetCursorScreenPos(descriptionPos);

    CherryGUI::GetFont()->Scale = 0.7;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(maxTextWidth);
    CherryGUI::TextWrapped(description.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 smallRectPos = ImVec2(
        cursorPos.x + fixedSize.x - versionBoxWidth - padding, cursorPos.y + fixedSize.y - versionBoxHeight - padding);

    Cherry::SetNextComponentProperty("color", "#353535");
    CherryKit::Separator();

    CherryStyle::RemoveYMargin(20.0f);
    CherryGUI::BeginChild(
        "###action_bar", ImVec2(200, 50), false, ImGuiWindowFlags_NoScrollbar || ImGuiWindowFlags_NoScrollWithMouse);
    CherryStyle::AddMarginX(5.0f);
    Cherry::SetNextComponentProperty("color_text", "#888888");

    CherryGUI::SameLine();
    if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/trash.png"))->GetData("isClicked") == "true") {
      content_to_delete = content;
      delete_content_modal_opened = true;
    }

    CherryGUI::SameLine();

    if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
            ->GetData("isClicked") == "true") {
      VortexMaker::OpenFolderInFileManager(content->m_path);
    }

    CherryGUI::EndChild();

    float windowVisibleX2 = CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
    if (cursorPos.x + fixedSize.x < windowVisibleX2)
      CherryGUI::SameLine();

    CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

    return pressed;
  }

  void ContentManager::ModulesRender() {
    // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
    Cherry::PushFont("ClashBold");
    CherryNextProp("color_text", "#797979");
    CherryKit::TitleOne("QSf");
    Cherry::PopFont();
    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }

  ContentManager::ContentManager(const std::string &name) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png"));

    m_AppWindow->SetClosable(true);
    m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

    m_AppWindow->m_TabMenuCallback = []() {
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);

    m_SelectedChildName = "Templates";
    m_RecentProjects = GetMostRecentProjects(VortexMaker::GetCurrentContext()->IO.sys_projects, 4);

    this->AddChild(
        "Help",
        ContentManagerChild(
            [this]() {
              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#BCBCBC");
              CherryKit::TitleFive("Understanding the Vortex approach.");
              Cherry::PopFont();

              CherryNextProp("color", "#252525");
              CherryKit::Separator();
              CherryNextProp("color_text", "#999999");
              CherryKit::TextWrapped(
                  "Vortex allows you to install contents directly onto your system, making them accessible in your projects "
                  "or at the project creation. Static content consists of elements you can create, import, and export—often "
                  "with the help of modules or plugins. These contents are frequently interactive within the content "
                  "browser.");
              if (CherryKit::ButtonImageTextImage(
                      "Learn and Documentation",
                      Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
                      Cherry::GetPath("resources/imgs/weblink.png"))
                      ->GetData("isClicked") == "true") {
                VortexMaker::OpenURL("https://vortex.infinite.si/learn");
              }
            },
            Cherry::GetPath("resources/imgs/help.png")));
    this->AddChild(
        "Contents",
        ContentManagerChild(
            [this]() {
              static std::vector<std::shared_ptr<Cherry::Component>> contents_block;

              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("All contents in the system");
              Cherry::PopFont();
              CherryGUI::SameLine();

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") ==
                  "true") {
                m_AssetFinder = AssetFinder::Create("Import content(s)", VortexMaker::getHomeDirectory());
                Cherry::ApplicationSpecification spec;

                std::string name = "Find content(s)";
                spec.Name = name;
                spec.MinHeight = 500;
                spec.MinWidth = 500;
                spec.Height = 600;
                spec.Width = 1150;
                spec.DisableTitle = false;
                spec.CustomTitlebar = true;
                spec.DisableWindowManagerTitleBar = true;
                spec.WindowOnlyClosable = false;
                spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
                spec.UniqueAppWindowName = m_AssetFinder->GetAppWindow()->m_Name;
                spec.UsingCloseCallback = true;
                spec.FavIconPath = Cherry::GetPath("resources/imgs/vbox.png");
                spec.IconPath = Cherry::GetPath("resources/imgs/vbox.png");
                spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
                spec.WindowSaves = false;
                spec.MenubarCallback = [this]() {
                  if (CherryGUI::BeginMenu("Window")) {
                    if (CherryGUI::MenuItem("Close")) {
                      Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
                    }
                    CherryGUI::EndMenu();
                  }
                };

                m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);

                if (!VortexMaker::GetCurrentContext()->IO.sys_contents_pools.empty()) {
                  m_AssetFinder->m_TargetPossibilities = VortexMaker::GetCurrentContext()->IO.sys_contents_pools;
                }

                m_AssetFinder->GetAppWindow()->SetVisibility(true);
                m_AssetFinder->m_ItemToReconize.push_back(std::make_shared<AssetFinderItem>(
                    VortexMaker::CheckContentInDirectory, "Content sample", "Content", Cherry::HexToRGBA("#B1FF31")));

                Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
              }

              CherryGUI::SameLine();
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Browse", Cherry::GetPath("resources/imgs/icons/misc/icon_net.png"))
                      ->GetData("isClicked") == "true") {
                m_WipNotification = true;
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              if (m_AssetFinder) {
                if (m_AssetFinder->m_GetFileBrowserPath) {
                  m_AssetFinder->m_GetFileBrowserPath = false;
                  m_FindedContents.clear();

                  // SearchModulesOnDirectory(ContentPath);
                  std::string pool;
                  if (!VortexMaker::GetCurrentContext()->IO.sys_contents_pools[m_AssetFinder->m_TargetPoolIndex].empty()) {
                    pool = VortexMaker::GetCurrentContext()->IO.sys_contents_pools[m_AssetFinder->m_TargetPoolIndex];
                  }

                  for (auto selected : m_AssetFinder->m_Selected) {
                    VortexMaker::InstallContentOnSystem(selected, pool);
                  }

                  contents_block.clear();

                  VortexMaker::LoadSystemContents(VortexMaker::GetCurrentContext()->IO.sys_contents);

                  m_AssetFinder->GetAppWindow()->SetVisibility(false);
                  m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
                }
              }

              CherryKit::ModalSimple("Import content(s)", &import_content_modal_opened, [this]() {

              });

              CherryKit::ModalSimple("Delete a content", &delete_content_modal_opened, []() {
                if (!content_to_delete) {
                  delete_content_modal_opened = true;
                  return;
                }

                CherryKit::TextWrapped(
                    "Important: This action will not delete selected plugins from projects you've got, this action will "
                    "only "
                    "uninstall project available from your system. To delete plugins of your projects, go on the project "
                    "editor, "
                    "plugins manager and delete plugins manually.");
                CherryKit::Separator();

                if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                        ->GetData("isClicked") == "true") {
                  delete_content_modal_opened = false;
                }
                CherryGUI::SameLine();

                if (CherryKit::ButtonImageText("Confirm Delete", Cherry::GetPath("resources/imgs/trash.png"))
                        ->GetData("isClicked") == "true") {
                  // TODO
                  VortexMaker::DeleteSystemContent(content_to_delete->m_name, content_to_delete->m_contentid);
                  contents_block.clear();
                  VortexMaker::LoadSystemContents(VortexMaker::GetCurrentContext()->IO.sys_contents);
                  delete_content_modal_opened = false;
                }
              });

              if (contents_block.empty()) {
                for (auto syscontent : VortexMaker::GetCurrentContext()->IO.sys_contents) {
                  contents_block.push_back(CherryKit::BlockVerticalCustom(
                      Cherry::IdentifierPattern(Cherry::IdentifierProperty::CreateOnly, syscontent->m_name),
                      m_CreateProjectCallback,
                      120.0f,
                      160.0f,
                      {
                          [this, syscontent]() {
                            ItemContentCard(
                                syscontent->m_name,
                                syscontent->m_path,
                                syscontent->m_name,
                                syscontent->m_contentid,
                                false,
                                Application::CookPath("resources/imgs/icons/files/icon_picture_file.png"),
                                IM_COL32(56, 56, 56, 150),
                                IM_COL32(50, 50, 50, 255),
                                IM_COL32(50, 50, 50, 255),
                                100.0f,
                                5.0f,
                                syscontent);
                          },

                          /*[syscontent]() {

                          },*/
                      }));
                }
              }

              // Draw grid with blocks
              Cherry::PushPermanentProperty("block_border_radius", "6.0");
              CherryKit::GridSimple(270.0f, 270.0f, &contents_block);
              Cherry::PopPermanentProperty();
            },
            Cherry::GetPath("resources/imgs/brick.png")));
    this->AddChild(
        "Templates",
        ContentManagerChild(
            [this]() {
              static std::vector<std::shared_ptr<Cherry::Component>> templates_block;
              static bool delete_template_modal_opened = false;
              static bool import_template_modal_opened = false;
              static std::shared_ptr<TemplateInterface> template_to_delete = nullptr;

              Cherry::PushFont("ClashBold");
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive("All templates in the system");
              Cherry::PopFont();
              CherryGUI::SameLine();
              CherryKit::TooltipTextCustom("(?)", []() { CherryKit::TitleFour("em : Editor templates"); });

              CherryGUI::SameLine();
              CherryStyle::AddMarginX(10.0f);
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");
              if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/base/add.png"))->GetData("isClicked") ==
                  "true") {
                m_AssetFinder = AssetFinder::Create("Import templates(s)", VortexMaker::getHomeDirectory());
                Cherry::ApplicationSpecification spec;

                std::string name = "Find templates(s)";
                spec.Name = name;
                spec.MinHeight = 500;
                spec.MinWidth = 500;
                spec.Height = 600;
                spec.Width = 1150;
                spec.DisableTitle = false;
                spec.CustomTitlebar = true;
                spec.DisableWindowManagerTitleBar = true;
                spec.WindowOnlyClosable = false;
                spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
                spec.UniqueAppWindowName = m_AssetFinder->GetAppWindow()->m_Name;
                spec.UsingCloseCallback = true;
                spec.FavIconPath = Cherry::GetPath("resources/imgs/vtemplate.png");
                spec.IconPath = Cherry::GetPath("resources/imgs/vtemplate.png");
                spec.CloseCallback = [this]() { Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow()); };
                spec.WindowSaves = false;
                spec.MenubarCallback = [this]() {
                  if (CherryGUI::BeginMenu("Window")) {
                    if (CherryGUI::MenuItem("Close")) {
                      Cherry::DeleteAppWindow(m_AssetFinder->GetAppWindow());
                    }
                    CherryGUI::EndMenu();
                  }
                };

                m_AssetFinder->GetAppWindow()->AttachOnNewWindow(spec);

                if (!VortexMaker::GetCurrentContext()->IO.sys_templates_pools.empty()) {
                  m_AssetFinder->m_TargetPossibilities = VortexMaker::GetCurrentContext()->IO.sys_templates_pools;
                }

                m_AssetFinder->GetAppWindow()->SetVisibility(true);
                m_AssetFinder->m_ItemToReconize.push_back(std::make_shared<AssetFinderItem>(
                    VortexMaker::CheckTemplateInDirectory, "Template sample", "Template", Cherry::HexToRGBA("#B1FF31")));

                Cherry::AddAppWindow(m_AssetFinder->GetAppWindow());
              }

              CherryGUI::SameLine();
              Cherry::SetNextComponentProperty("padding_x", "8");
              Cherry::SetNextComponentProperty("padding_y", "4");

              if (CherryKit::ButtonImageText("Browse", Cherry::GetPath("resources/imgs/icons/misc/icon_net.png"))
                      ->GetData("isClicked") == "true") {
                m_WipNotification = true;
              }

              CherryNextProp("color", "#252525");
              CherryKit::Separator();

              if (m_AssetFinder) {
                if (m_AssetFinder->m_GetFileBrowserPath) {
                  m_AssetFinder->m_GetFileBrowserPath = false;
                  m_FindedTemplates.clear();

                  // SearchModulesOnDirectory(ContentPath);
                  std::string pool;
                  if (!VortexMaker::GetCurrentContext()->IO.sys_templates_pools[m_AssetFinder->m_TargetPoolIndex].empty()) {
                    pool = VortexMaker::GetCurrentContext()->IO.sys_templates_pools[m_AssetFinder->m_TargetPoolIndex];
                  }

                  for (auto selected : m_AssetFinder->m_Selected) {
                    VortexMaker::InstallTemplateOnSystem(selected, pool);
                  }

                  templates_block.clear();

                  VortexMaker::LoadSystemTemplates(VortexMaker::GetCurrentContext()->IO.sys_templates);

                  m_AssetFinder->GetAppWindow()->SetVisibility(false);
                  m_AssetFinder->GetAppWindow()->SetParentWindow(Cherry::Application::GetCurrentRenderedWindow()->GetName());
                }
              }

              CherryKit::ModalSimple("Import template(s)", &import_template_modal_opened, [this]() {

              });

              CherryKit::ModalSimple("Delete a template", &delete_template_modal_opened, []() {
                if (!template_to_delete) {
                  delete_template_modal_opened = true;
                  return;
                }

                CherryKit::TextWrapped(
                    "Important: This action will not delete selected plugins from projects you've got, this action will "
                    "only "
                    "uninstall project available from your system. To delete plugins of your projects, go on the project "
                    "editor, "
                    "plugins manager and delete plugins manually.");
                CherryKit::Separator();

                if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                        ->GetData("isClicked") == "true") {
                  delete_template_modal_opened = false;
                }
                CherryGUI::SameLine();

                if (CherryKit::ButtonImageText("Confirm Delete", Cherry::GetPath("resources/imgs/trash.png"))
                        ->GetData("isClicked") == "true") {
                  // TODO
                  VortexMaker::DeleteSystemTemplate(template_to_delete->m_name, template_to_delete->m_version);
                  templates_block.clear();
                  VortexMaker::LoadSystemTemplates(VortexMaker::GetCurrentContext()->IO.sys_templates);
                  delete_template_modal_opened = false;
                }
              });

              if (templates_block.empty()) {
                for (auto systemplate : VortexMaker::GetCurrentContext()->IO.sys_templates) {
                  templates_block.push_back(CherryKit::BlockVerticalCustom(
                      Cherry::IdentifierPattern(Cherry::IdentifierProperty::CreateOnly, systemplate->m_name),
                      m_CreateProjectCallback,
                      200.0f,
                      110.0f,
                      {
                          [systemplate]() {
                            CherryStyle::AddMarginX(5.0f);
                            CherryStyle::AddMarginY(5.0f);
                            CherryKit::ImageLocal(Cherry::GetPath(systemplate->m_logo_path), 28.0f, 28.0f);

                            CherryGUI::SameLine();
                            Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
                            CherryStyle::AddMarginY(2.0f);
                            std::string label = "####" + systemplate->m_name;
                            CherryKit::TextSimple(systemplate->m_proper_name);
                          },
                          [systemplate]() {
                            CherryStyle::AddMarginX(5.0f);
                            CherryStyle::RemoveYMargin(5.0f);

                            Cherry::SetNextComponentProperty("color_text", "#686868");
                            if (systemplate->m_type == "project") {
                              CherryKit::TextSimple("Project template");

                            } else {
                              CherryKit::TextSimple("Template");
                            }

                            Cherry::SetNextComponentProperty("color_text", "#686868");
                            CherryKit::TextSimple(systemplate->m_version);

                            CherryGUI::SameLine();

                            Cherry::SetNextComponentProperty("color_text", "#9B9B9B");
                            std::string wrapped_description = systemplate->m_description;
                            if (wrapped_description.size() > 75) {
                              wrapped_description = wrapped_description.substr(0, 72);
                              wrapped_description.append("...");
                            }
                            CherryKit::TextWrapped(wrapped_description);
                          },
                          [systemplate]() {
                            Cherry::SetNextComponentProperty("color", "#353535");
                            CherryKit::Separator();

                            CherryStyle::RemoveYMargin(20.0f);
                            CherryGUI::BeginChild("###action_bar", ImVec2(200, 50), false, ImGuiWindowFlags_NoScrollbar);
                            CherryStyle::AddMarginX(5.0f);
                            Cherry::SetNextComponentProperty("color_text", "#888888");

                            CherryGUI::SameLine();
                            if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/trash.png"))
                                    ->GetData("isClicked") == "true") {
                              std::cout << "True" << std::endl;

                              template_to_delete = systemplate;
                              delete_template_modal_opened = true;
                            }

                            CherryGUI::SameLine();

                            if (CherryKit::ButtonImageText(
                                    "", Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png"))
                                    ->GetData("isClicked") == "true") {
                              VortexMaker::OpenFolderInFileManager(systemplate->m_path);
                            }

                            CherryGUI::EndChild();
                          },
                      }));
                }
              }

              // Draw grid with blocks
              Cherry::PushPermanentProperty("block_border_radius", "6.0");
              CherryKit::GridSimple(270.0f, 270.0f, &templates_block);
              Cherry::PopPermanentProperty();
            },
            Cherry::GetPath("resources/imgs/template.png")));

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
  }

  std::vector<std::shared_ptr<EnvProject>> ContentManager::GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>> &projects,
      size_t maxCount) {
    auto sortedProjects = projects;
    std::sort(
        sortedProjects.begin(),
        sortedProjects.end(),
        [](const std::shared_ptr<EnvProject> &a, const std::shared_ptr<EnvProject> &b) {
          return a->lastOpened > b->lastOpened;
        });

    if (sortedProjects.size() > maxCount) {
      sortedProjects.resize(maxCount);
    }
    return sortedProjects;
  }

  void ContentManager::AddChild(const std::string &child_name, const ContentManagerChild &child) {
    m_Childs[child_name] = child;
  }

  void ContentManager::RemoveChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      m_Childs.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &ContentManager::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<ContentManager> ContentManager::Create(const std::string &name) {
    auto instance = std::shared_ptr<ContentManager>(new ContentManager(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void ContentManager::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  ContentManagerChild *ContentManager::GetChild(const std::string &child_name) {
    auto it = m_Childs.find(child_name);
    if (it != m_Childs.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void ContentManager::Render() {
    CherryKit::NotificationButton(
        &m_WipNotification,
        4,
        "info",
        "Work in progress",
        "This feature is not available yet. Thanks for your patience.",
        []() { });

    std::string label = "left_pane" + m_AppWindow->m_Name;
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true);

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
    CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/contents.png")), ImVec2(280, 142));

    // CherryStyle::SetPadding(7.0f);

    for (const auto &child : m_Childs) {
      if (child.first == m_SelectedChildName) {
        // opt.hex_text_idle = "#FFFFFFFF";
      } else {
        // opt.hex_text_idle = "#A9A9A9FF";
      }
      std::string child_name;

      if (child.first.rfind("?loc:", 0) == 0) {
        std::string localeName = child.first.substr(5);
        child_name = Cherry::GetLocale(localeName) + "####" + localeName;
      } else {
        child_name = child.first;
      }

      CherryNextProp("color_bg", "#00000000");
      CherryNextProp("color_border", "#00000000");
      CherryNextProp("padding_x", "2");
      CherryNextProp("padding_y", "2");
      CherryNextProp("size_x", "20");
      CherryNextProp("size_y", "20");
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);

      if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.second.LogoPath)
              ->GetData("isClicked") == "true") {
        m_SelectedChildName = child.first;
      }

      if (child_name == "Templates")  // TODO (WARN) : Prepare to locales
      {
        CherryGUI::SameLine();
        Cherry::SetNextComponentProperty("color_text", "#676767");
        CherryStyle::AddMarginY(3.5f);
        CherryKit::TextSimple("(" + std::to_string(VortexMaker::GetCurrentContext()->IO.sys_templates.size()) + ")");
        CherryStyle::RemoveYMargin(3.5f);
      } else if (child_name == "Contents")  // TODO (WARN) : Prepare to locales
      {
        CherryGUI::SameLine();
        Cherry::SetNextComponentProperty("color_text", "#676767");
        CherryStyle::AddMarginY(3.5f);
        CherryKit::TextSimple("(" + std::to_string(VortexMaker::GetCurrentContext()->IO.sys_contents.size()) + ")");
        CherryStyle::RemoveYMargin(3.5f);
      }

      // if (Cherry::TextButtonUnderline(child_name.c_str(), true, opt))
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(4);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

    if (!m_SelectedChildName.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

      if (CherryGUI::BeginChild(
              "ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        auto child = GetChild(m_SelectedChildName);

        if (child) {
          std::function<void()> pannel_render = child->RenderCallback;
          if (pannel_render) {
            pannel_render();
          }
        }
      }
      CherryGUI::EndChild();

      CherryGUI::PopStyleVar(2);
    }

    CherryGUI::EndGroup();
  }

}  // namespace VortexLauncher
