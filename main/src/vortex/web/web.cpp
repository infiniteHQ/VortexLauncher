#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

#include "../../../../lib/restcpp/include/restclient-cpp/restclient.h"

// TODO Estimate the cpu arch and platform

VORTEX_API void VortexMaker::UpdateVortexLauncherWebData()
{
    VxContext &ctx = *CVortexMaker;
    RestClient::Response r = RestClient::get("http://api.infinite.si:9000/api/vortexupdates/get_vl_versions?dist=stable_windows&arch=x86_64");

    if (r.code != 200)
    {
        return;
    }

    nlohmann::json jsonData = nlohmann::json::parse(r.body);
    std::string highestVersion = "0.0.0";

    for (const auto &item : jsonData)
    {
        VortexLauncherVersion version;
        version.dist = item["dist"].get<std::string>();
        version.arch = item["arch"].get<std::string>();
        version.created_at = item["created_at"].get<std::string>();

        std::string values_str = item["values"];
        nlohmann::json jsonValues = nlohmann::json::parse(values_str);

        version.sum = jsonValues["sum"].get<std::string>();
        version.path = jsonValues["path"].get<std::string>();
        version.version = jsonValues["version"].get<std::string>();

        ctx.latest_launcher_version = version;

        if (VortexMaker::IsVersionGreater(highestVersion, ctx.version))
        {
            ctx.launcher_update_available = true;
        }
    }

    std::cout << "________________ LATEST LAUNCHER ______________________" << std::endl;
    std::cout << ctx.latest_launcher_version.version << " /// " << ctx.latest_launcher_version.path << " /// " << ctx.latest_launcher_version.created_at << std::endl;
    std::cout << "______________________________________" << std::endl;
}
VORTEX_API void VortexMaker::UpdateVortexWebData()
{
    RestClient::Response r = RestClient::get("http://api.infinite.si:9000/api/vortexupdates/get_filtered_v_versions?platform=linux&dist=stable");

    if (r.code != 200)
    {
        return;
    }

    nlohmann::json jsonData = nlohmann::json::parse(r.body);
    std::vector<VortexVersion> latest_vortex_versions;
    VortexVersion latest_vortex_version;
    std::string highestVersion = "0.0.0";

    for (const auto &item : jsonData)
    {
        VortexVersion version;
        version.version = item["version"].get<std::string>();
        version.name = item["name"].get<std::string>();
        version.path = item["link"].get<std::string>();
        version.date = item["date"].get<std::string>();
        version.banner = ""; 
        version.sum = ""; 

        latest_vortex_versions.push_back(version);

        if (VortexMaker::IsVersionGreater(highestVersion, version.version))
        {
            highestVersion = version.version;
            latest_vortex_version = version;
        }
    }

    VxContext &ctx = *CVortexMaker;
        ctx.latest_vortex_versions = latest_vortex_versions;
        ctx.latest_vortex_version = latest_vortex_version;

    VortexVersion saved_latest_version = VortexMaker::CheckLatestVortexVersion();

    if (VortexMaker::IsVersionGreater(saved_latest_version.version, latest_vortex_version.version))
    {
        ctx.vortex_update_available = true;
    }
    else
    {
        ctx.vortex_update_available = false;
    }
        VortexMaker::PostLatestVortexVersion(latest_vortex_version);

    std::cout << "________________ ALL ______________________" << std::endl;
    for (auto version : ctx.latest_vortex_versions)
    {
        std::cout << version.name << " /// " << version.version << " /// " << version.path << std::endl;
    }
    std::cout << "______________________________________" << std::endl;

    std::cout << "________________ LATEST ______________________" << std::endl;
    std::cout << ctx.latest_vortex_version.name << " /// " << ctx.latest_vortex_version.version << " /// " << ctx.latest_vortex_version.path << std::endl;
    std::cout << "______________________________________" << std::endl;
}