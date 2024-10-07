#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../main/include/modules/install.h"
#include "../../../../../main/include/modules/load.h"
#include "../../../../../main/include/modules/delete.h"
#include "../../../../../main/include/templates/delete.h"
#include "../../../../../main/include/templates/load.h"

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

class SystemSettings
{
public:
    SystemSettings();

    void OnImGuiRender(const std::string &parent, std::function<void(ImGuiWindow *)> controller);
    void menubar();
    void addModuleModal();

    void RefreshRender(const std::shared_ptr<SystemSettings> &instance);

    void Refresh();
    void Update();
    std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
    {
        return m_AppWindow;
    }

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

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;

    std::thread receiveThread;
    std::vector<std::shared_ptr<TemplateInterface>> project_templates;
    bool CollapseAll = false;
};

#endif // SYSTEMSETTINGS_H