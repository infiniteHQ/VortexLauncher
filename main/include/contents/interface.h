#include "../vortex.h"

#ifndef CONTENT_INTERFACE_H
#define CONTENT_INTERFACE_H

class ContentInterface {
 public:
  std::string m_type;  // module_content, plugin_content
  std::string m_proper_name;
  std::string m_name;  // unique
  std::string m_description;
  std::string m_contentbrowsercolor;
  std::string m_contentid;
  std::string m_path;
  bool m_selected = false;
};

#endif  // CONTENT_INTERFACE_H