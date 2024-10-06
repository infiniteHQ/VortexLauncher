#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/templates/load.h"

namespace VortexMaker
{
    VORTEX_API void LoadSystemTemplates(std::vector<std::shared_ptr<TemplateInterface>> &sys_templates)
    {
        // Get the home directory
        std::string homeDir = VortexMaker::getHomeDirectory();

        // Module path on the system
        std::string path = homeDir + "/.vx/templates";

        // Verify if the path exist, if not, create it
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }

        // Clear actual templates array
        sys_templates.clear();

        // Search for module files recursively in the directory
        auto module_files = VortexMaker::SearchSystemFiles(path, "template.json");

        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                std::string module_path = file.substr(0, file.find_last_of("/"));

                std::shared_ptr<TemplateInterface> new_template = std::make_shared<TemplateInterface>();

                // Try to fetch module informations from module.json
                try
                {
                    new_template->m_name = json_data["name"].get<std::string>();
                    new_template->m_proper_name = json_data["proper_name"].get<std::string>();
                    new_template->m_type = json_data["type"].get<std::string>();
                    new_template->m_description = json_data["description"].get<std::string>();
                    new_template->m_picture = json_data["picture"].get<std::string>();
                    new_template->m_logo_path = module_path + "/" + new_template->m_picture;
                    new_template->m_path = module_path + "/";
                    new_template->m_author = json_data["author"].get<std::string>();
                    new_template->m_group = json_data["group"].get<std::string>();
                    new_template->m_tarball = json_data["tarball"].get<std::string>();
                    new_template->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

                    for (auto dep : json_data["deps"])
                    {
                        std::shared_ptr<TemplateDep> dependence = std::make_shared<TemplateDep>();
                        dependence->name = dep["name"].get<std::string>();
                        dependence->type = dep["type"].get<std::string>();
                        for (auto version : dep["versions"])
                        {
                            dependence->supported_versions.push_back(version);
                        }
                        new_template->m_dependencies.push_back(dependence);
                    }
                }
                catch (std::exception e)
                {
                    VortexMaker::LogError("Core", e.what());
                }

                VXINFO("Template registered in system ", new_template->m_name + " loaded with success !")

                // Store the module instance and handle
                sys_templates.push_back(new_template);
            }
            catch (const std::exception &e)
            {
                // Print error if an exception occurs
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

} // namespace VortexMaker