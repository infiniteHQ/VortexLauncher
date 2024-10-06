#include "../vortex.h"

#ifndef TEMPLATE_H
#define TEMPLATE_H

struct TemplateDep
{
    std::string type; // other template, em, plugin, etc...
    std::string name;
    std::vector<std::string> supported_versions;
    bool satisfied;
};

class TemplateInterface
{
    public:
    std::string m_type; // project, module:task_base, etc..
    std::string m_proper_name;
    std::string m_name; // unique
    std::string m_description;
    std::string m_group;
    std::string m_tarball;
    std::string m_logo_path;
    std::string m_picture;
    std::string m_path;
    std::string m_author;
    std::string m_version;
    std::string m_path_to_deploy;
    std::vector<std::string> m_contributors;
    std::vector<std::shared_ptr<TemplateDep>> m_dependencies; 
};

#endif // TEMPLATE_H