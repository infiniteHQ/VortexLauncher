#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::CreateSessionTopic(const std::string &post_topic)
{
    std::string topic_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + post_topic;

    {
        std::string cmd = "mkdir " + topic_path;
        if (std::system(cmd.c_str()) == 0)
        {
            VortexMaker::LogInfo("Core", "Session \"" + post_topic + "\" is created with success !");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session \"" + post_topic + "\"!");
        }
    }

    std::string crash_path = topic_path + "/crash";

    {
        std::string cmd = "mkdir " + crash_path;
        if (std::system(cmd.c_str()) == 0)
        {
            VortexMaker::LogInfo("Core", "Session crash folder of \"" + post_topic + "\" is created with success !");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session crash folder of \"" + post_topic + "\"!");
        }
    }

    std::string log_path = topic_path + "/logs";

    {
        std::string cmd = "mkdir " + log_path;
        if (std::system(cmd.c_str()) == 0)
        {
            VortexMaker::LogInfo("Core", "Session logs folder of \"" + post_topic + "\" is created with success !");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session logs folder of \"" + post_topic + "\"!");
        }
    }

    {
        std::string cmd = "touch " + crash_path + "/core_dump.txt";
        if (std::system(cmd.c_str()) == 0)
        {
            VortexMaker::LogInfo("Core", "Session crash folder of \"" + post_topic + "\" is created with success !");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session crash folder of \"" + post_topic + "\"!");
        }
    }

    {
        std::string cmd = "touch " + crash_path + "/log_dump.txt";
        if (std::system(cmd.c_str()) == 0)
        {
            VortexMaker::LogInfo("Core", "Session crash folder of \"" + post_topic + "\" is created with success !");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session crash folder of \"" + post_topic + "\"!");
        }
    }

    {
        std::string cmd = "touch " + topic_path + "/vortex.json";
        if (std::system(cmd.c_str()) == 0)
        {
            VortexMaker::LogInfo("Core", "Session topic files of \"" + post_topic + "\" is created with success !");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create topic files of \"" + post_topic + "\"!");
        }
    }
}