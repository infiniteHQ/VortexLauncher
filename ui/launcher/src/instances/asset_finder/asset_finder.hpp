#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H
#include "../../utils.hpp"

using namespace Cherry;

class AssetFinderItem {
 public:
  bool (*f_Detect)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 m_LineColor;
  std::string m_Description;

  AssetFinderItem(
      bool (*detect_function)(const std::string &path),
      const std::string &name,
      const std::string &description,
      const ImVec4 &line_color)
      : m_Name(name),
        m_Description(description),
        f_Detect(detect_function),
        m_LineColor(line_color) { };
};

struct AssetFinderChild {
  std::function<void()> m_Child;
  std::string m_Name;
  bool m_Disabled = true;
  float m_DefaultSize = 0.0f;
  float m_MinSize;
  float m_MaxSize;
  float m_Size = 200.0f;
  float m_Ratio = 0.0f;
  bool m_Resizable = true;
  bool m_ResizeDisabled = false;
  bool m_Initialized = false;
  bool m_InitializedSec = false;
  bool m_InitializedTh = false;
  ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

  void Enable() {
    m_Disabled = true;
  }

  void Disable() {
    m_Disabled = false;
  }

  AssetFinderChild(
      const std::string &name,
      const std::function<void()> &child,
      const float &default_size = 0.0f,
      const bool &resize_disabled = false,
      const float &min_size = 0.0f,
      const float &max_size = 0.0f)
      : m_Name(name),
        m_Child(child),
        m_ResizeDisabled(resize_disabled),
        m_DefaultSize(default_size),
        m_MinSize(min_size),
        m_MaxSize(max_size) {
  }
};

class AssetFinder : public std::enable_shared_from_this<AssetFinder> {
 public:
  AssetFinder(const std::string &name, const std::string &start_path);
  // AssetFinder(const std::string &name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<AssetFinder> Create(const std::string &name, const std::string &base_path);
  void SetupRenderCallback();
  void Render();

  bool MyButton(
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
      float borderRadius);
  void AddChild(const AssetFinderChild &child);
  void ChangeDirectory(const std::filesystem::path &newDirectory);
  void GoBack();
  void GoForward();
  void DrawPathBar(const std::string &path);

  void RenderSideBar();
  void RenderContentBar();

  void Select(const std::string &path) {
    for (auto already : m_Selected) {
      if (path == already) {
        return;
      }
    }
    m_Selected.push_back(path);
  }

  bool ItemCard(
      const std::string &name,
      const std::string &path,
      const std::string &description,
      const std::string &size,
      bool selected,
      const std::string &logo,
      ImU32 bgColor = IM_COL32(100, 100, 100, 255),
      ImU32 borderColor = IM_COL32(150, 150, 150, 255),
      ImU32 lineColor = IM_COL32(255, 255, 0, 255),
      float maxTextWidth = 100.0f,
      float borderRadius = 5.0f);

  // To move in components
  void DrawFolderIcon(ImVec2 pos, ImVec2 size, ImU32 color);
  void FolderButton(const char *id, ImVec2 size, ImU32 color, const std::string &path);
  void DrawHierarchy(std::filesystem::path path, bool isDir, const std::string &label = "");

  void SetDefaultFolderColor(const std::string &hex);

  std::string GetFileBrowserFolderColor(const std::string &path) {
    for (auto &colored_folder : m_FolderColors) {
      if (colored_folder.first == path) {
        return colored_folder.second;
      }
    }

    return "#997D44FF";
  }

  bool IsPathFavorite(const std::string &path) {
    return false;
  };
  void SetColoredFolder(const std::string &path, const std::string &hex_color) { };

  std::vector<AssetFinderChild> m_Childs;
  std::string m_CurrentDirectory;

  std::function<bool(const std::string &)> m_IsValidPathCallback;
  std::vector<std::string> m_SelectedPath;
  int m_TargetPoolIndex;
  std::vector<std::string> m_TargetPossibilities;
  std::vector<std::shared_ptr<AssetFinderItem>> m_ItemToReconize;

  std::vector<std::string> m_Selected;
  bool m_GetFileBrowserPath = false;

 private:
  bool opened;

  bool m_ThumbnailMode = true;
  bool m_ListMode = false;

  bool m_ShowTypes = false;
  bool m_ShowSizes = false;

  // !!!
  bool m_ShowFolderPannel = true;
  bool m_ShowFilterPannel = false;
  bool m_ShowThumbnailVisualizer = false;
  bool m_ShowSelectionQuantifier = false;

  std::filesystem::path m_BaseDirectory;

  std::stack<std::filesystem::path> m_BackHistory;
  std::stack<std::filesystem::path> m_ForwardHistory;

  std::vector<std::string> m_CopySelection;
  std::vector<std::string> m_CutSelection;

  std::string m_DefaultFolderColor;

  // Path/Color
  std::vector<std::pair<std::string, std::string>> m_FolderColors;
  std::vector<std::string> m_FavoriteFolders;
  std::vector<std::string> m_Pools;

  std::vector<std::filesystem::path> m_Favorites;

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;

  std::function<void(const std::string &)> m_DeletePathCallback;
  std::function<void(const std::vector<std::string> &)> m_CopyPathsCallback;
  std::function<void(const std::vector<std::string> &)> m_PastePathsCallback;
};

#endif  // UIKIT_V1_AIO_CONTENT_FileBrowser