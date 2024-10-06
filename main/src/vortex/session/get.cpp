#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * Directory places :
 * Master session informations     | /[post_topic]/master_session.json
 * Last module informations        | /[post_topic]/last_module.json
*/

/**
 * @brief Get json the last module of the last session informations.
 * 
 * This function is usefull to see informations of a dead vortex processus.
 * 
 * @param post_topic The topic where post the state of context.
 */
VORTEX_API nlohmann::json VortexMaker::GetLastModuleOfLastSession(const std::string& post_topic)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Init the path of the sessions data.
    std::string session_path = VortexMaker::getHomeDirectory() + "/.vx/sessions";

    // Init the path of current session data folder.
    std::string current_session_path = session_path + "/.vx/sessions/" + post_topic + "/last_module.json";

    std::ifstream file(current_session_path);

    nlohmann::json jsonContenu;

    if (file)
    {
        file >> jsonContenu;
    }

    return jsonContenu;
}

/**
 * @brief Post a session state of current context
 * 
 * This function can post a state of the current context to prevent crashs & log everything.
 * 
 * @param post_topic The topic where post the state of context.
 */
VORTEX_API nlohmann::json VortexMaker::GetLastSession(const std::string& post_topic)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Init the path of the sessions data.
    std::string session_path = VortexMaker::getHomeDirectory() + "/.vx/sessions";

    // Init the path of current session data folder.
    std::string current_session_path = session_path + "/.vx/sessions/" + post_topic + "/master_session.json";

    std::ifstream file(current_session_path);

    nlohmann::json jsonContenu;

    if (file)
    {
        file >> jsonContenu;
    }

    return jsonContenu;
}