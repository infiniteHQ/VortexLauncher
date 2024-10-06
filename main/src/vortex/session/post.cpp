#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * Directory places :
 * Vortex session informations     | /[post_topic]/vortex.json
 * Master session informations     | /[post_topic]/master_session.json
 * Last module informations        | /[post_topic]/last_module.json
*/

/**
 * @brief Post a session state of current context
 * 
 * This function can post a state of the current context to prevent crashs & log everything.
 * 
 * @param post_topic The topic where post the state of context.
 */
VORTEX_API void VortexMaker::PostSessionState(const std::string& post_topic)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Init the path of the sessions data.
    std::string session_path = VortexMaker::getHomeDirectory() + "/.vx/sessions";

    // Init the path of current session data folder.
    std::string current_session_path = session_path + "/.vx/sessions/" + post_topic;

    // Create the session folder if not exist
    VortexMaker::createFolderIfNotExists(session_path);

    // Create the current session folder if not exist
    VortexMaker::createFolderIfNotExists(current_session_path);

    if(!ctx.state.master_initialized)
    {
        nlohmann::json json;
        json["version"] = ctx.version;

        // Store JSON into vortex.config file
        VortexMaker::createJsonFileIfNotExists(session_path + "/vortex.json", json);

        ctx.state.master_initialized = true;
    }

    if(ctx.state.logs_modified)
    {
    }

    if(ctx.state.last_used_module_modified)
    {
        // Update logs metrics (logs journal are handle by spdlog)

        nlohmann::json json;
        json["name"] = ctx.state.last_used_module->m_name;
        json["version"] = ctx.state.last_used_module->m_version;
        json["author"] = ctx.state.last_used_module->m_author;

        // Store JSON into vortex.config file
        VortexMaker::createJsonFileIfNotExists(session_path + "/last_module.json", json);
    }
}

/**
 * @brief Post a core dump of a Vortex process
 * 
 * This function can post a state of the current context to prevent crashs & log everything.
 * 
 * @param post_topic The topic where post the state of context.
 */
VORTEX_API void VortexMaker::PostSessionCoreDump(const std::string& post_topic)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Init the path of the sessions data.
    std::string session_path = VortexMaker::getHomeDirectory() + "/.vx/sessions";

    // Init the path of current session data folder.
    std::string current_session_path = session_path + "/.vx/sessions/" + post_topic;

    // Create the session folder if not exist
    VortexMaker::createFolderIfNotExists(session_path);

    // Create the current session folder if not exist
    VortexMaker::createFolderIfNotExists(current_session_path);

}