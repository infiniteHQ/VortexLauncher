#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../lib/cherry/cherry.hpp"


#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

class ProjectManager
{
public:
	ProjectManager(VxContext *_ctx, const std::string& parent);

	void OnImGuiRender(const std::string& parent, std::function<void(ImGuiWindow*)> controller);
	void menubar();
	void addModuleModal();

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
	
	bool CollapseAll = false;
};

#endif // PROJECTMANAGER_H