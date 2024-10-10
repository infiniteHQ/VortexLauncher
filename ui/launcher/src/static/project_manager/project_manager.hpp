#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../utils.hpp"

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

class ProjectManager
{
public:
	ProjectManager();

	void addModuleModal();

	void RefreshRender(const std::shared_ptr<ProjectManager> &instance);

	std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
	{
		return m_AppWindow;
	}

	void Refresh();
	void Update();

	/**
	 * @brief Menu items
	 */
	void mainButtonsMenuItem();
	void filterMenuItem();
	void createMenuItem();
	void searchMenuItem();

	VxContext *ctx;
	std::string parent;

private:
	bool opened;

	std::shared_ptr<VxToolchain> latest_toolchain;

	std::thread receiveThread;
	std::vector<std::shared_ptr<TemplateInterface>> project_templates;
	std::shared_ptr<Cherry::Image> m_ListIcon;
	std::shared_ptr<Cherry::Image> m_RefreshIcon;
	std::shared_ptr<Cherry::Image> m_OpenIcon;
	std::shared_ptr<Cherry::Image> m_ProjectIcon;
	std::shared_ptr<Cherry::Image> m_AddIcon;
	std::shared_ptr<Cherry::Image> m_TrashIcon;



	// Create project components 
    std::shared_ptr<Cherry::SimpleTable> cp_SimpleTable;    
    std::shared_ptr<Cherry::DoubleKeyValString> cp_ProjectName;   
    std::shared_ptr<Cherry::DoubleKeyValString> cp_ProjectVersion;   
    std::shared_ptr<Cherry::DoubleKeyValString> cp_ProjectAuthor;   
    std::shared_ptr<Cherry::DoubleKeyValString> cp_ProjectDescription;   
    std::shared_ptr<Cherry::DoubleKeyValSimpleCombo> cp_ProjectPath;   
    std::shared_ptr<Cherry::DoubleKeyValBoolean> cp_ProjectOpen;   
	// Create project values
	std::shared_ptr<std::string> v_ProjectName;
	std::shared_ptr<std::string> v_ProjectVersion;
	std::shared_ptr<std::string> v_ProjectAuthor;
	std::shared_ptr<std::string> v_ProjectDescription;
	std::shared_ptr<bool> v_ProjectOpen;

	std::shared_ptr<Cherry::AppWindow> m_AppWindow;

	bool CollapseAll = false;
};

#endif // PROJECTMANAGER_H