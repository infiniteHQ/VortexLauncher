#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

// TODO Estimate the cpu arch and platform

VORTEX_API void VortexMaker::UpdateVortexLauncherWebData() {
  VxContext &ctx = *CVortexMaker;

  std::string plat = ctx.platform;
  std::string arch = ctx.arch;
  std::string dist = ctx.IO.sys_vortexlauncher_dist + "_" + plat;

  std::string url = "http://api.infinite.si:9000/api/vortexupdates/get_vl_versions?dist=" + dist + "&arch=" + arch;
  VortexMaker::LogWarn("URL", url);

  std::string body = ctx.net.GET(url);

  if (body.empty()) {
    return;
  }

  try {
    nlohmann::json jsonData = nlohmann::json::parse(body);

    for (const auto &item : jsonData) {
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
    }

    std::cout << "________________ LATEST LAUNCHER ______________________" << std::endl;
    std::cout << ctx.latest_launcher_version.version << " /// " << ctx.latest_launcher_version.path << " /// "
              << ctx.latest_launcher_version.created_at << std::endl;
    std::cout << "______________________________________" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
  }
}

VORTEX_API void VortexMaker::UpdateVortexNews(const std::vector<std::string> &topics) {
  auto *ctx = VortexMaker::GetCurrentContext();
  if (!ctx) {
    return;
  }

  for (const auto &topic : topics) {
    std::string url = "http://api.infinite.si:9000/api/vortexupdates/get_news?topic=" + topic;

    std::cout << "Fetching URL: " << url << std::endl;

    std::string responseBody = ctx->net.GET(url);
    if (responseBody.empty()) {
      std::cerr << "Failed to fetch news for topic: " << topic << std::endl;
      continue;
    }

    try {
      responseBody = responseBody.substr(1, responseBody.length() - 2);

      std::string unescapedResponse = nlohmann::json::parse("\"" + responseBody + "\"").get<std::string>();

      nlohmann::json jsonData = nlohmann::json::parse(unescapedResponse);

      VortexNews article;
      article.topic = topic;
      article.title = jsonData.value("title", "");
      article.description = jsonData.value("description", "");
      article.image_link = jsonData.value("image_link", "");
      article.news_link = jsonData.value("news_link", "");

      ctx->IO.news.push_back(article);
    } catch (const nlohmann::json::exception &e) {
      std::cerr << "JSON parsing error for topic " << topic << ": " << e.what() << std::endl;
      continue;
    }
  }
}

VORTEX_API void VortexMaker::UpdateVortexWebData() {
  VxContext &ctx = *CVortexMaker;

  std::string plat = ctx.platform;
  std::string arch = ctx.arch;

  nlohmann::json aggregatedJsonData = nlohmann::json::array();

  for (auto &dist : ctx.IO.sys_vortex_dists) {
    std::string url = "http://api.infinite.si:9000/api/vortexupdates/get_filtered_v_versions?platform=" + plat +
                      "&dist=" + dist + "&arch=" + arch;
    VortexMaker::LogWarn("URL", url);

    std::string body = ctx.net.GET(url);

    if (body.empty()) {
      VortexMaker::LogWarn("Failed to fetch data for dist", dist);
      continue;
    }

    try {
      nlohmann::json jsonData = nlohmann::json::parse(body);

      for (auto &item : jsonData) {
        item["dist"] = dist;
      }

      aggregatedJsonData.insert(aggregatedJsonData.end(), jsonData.begin(), jsonData.end());
    } catch (const std::exception &e) {
      VortexMaker::LogWarn("JSON parse error for dist " + dist, e.what());
      continue;
    }
  }

  std::shared_ptr<VortexVersion> latest_vortex_version;
  std::string highestVersion = "0.0.0";

  for (const auto &item : aggregatedJsonData) {
    std::shared_ptr<VortexVersion> version = std::make_shared<VortexVersion>();
    std::string proper_name = DEFAULT_VERSION_NAME;

    version->version = item["version"].get<std::string>();
    version->name = item["name"].get<std::string>();
    version->path = item["path"].get<std::string>();
    version->sum = item["sum"].get<std::string>();
    version->date = item["date"].get<std::string>();
    version->dist = item["dist"].get<std::string>();
    version->plat = plat;
    version->arch = arch;

    if (item.contains("proper_name") && !item["proper_name"].is_null()) {
      std::string value = item["proper_name"];
      if (!value.empty()) {
        proper_name = value;
      }
    }
    version->proper_name = proper_name;

    if (item.contains("values")) {
      auto values = nlohmann::json::parse(item["values"].get<std::string>());
      if (values.contains("image")) {
        version->banner = values["image"].get<std::string>();
      }
      if (values.contains("proper_name")) {
        version->proper_name = values["proper_name"].get<std::string>();
      }
    }

    ctx.IO.available_vortex_versions.push_back(version);

    if (VortexMaker::IsVersionGreater(highestVersion, version->version)) {
      highestVersion = version->version;
      latest_vortex_version = version;
    }
  }

  ctx.latest_vortex_version = latest_vortex_version;

  VortexVersion saved_latest_version = VortexMaker::CheckLatestVortexVersion();

  if (VortexMaker::IsVersionGreater(saved_latest_version.version, latest_vortex_version->version)) {
    ctx.vortex_update_available = true;
  } else {
    ctx.vortex_update_available = false;
  }

  VortexMaker::PostLatestVortexVersion(latest_vortex_version);

  std::cout << "________________ ALL ______________________" << std::endl;
  for (auto version : ctx.IO.available_vortex_versions) {
    std::cout << version->name << " /// " << version->version << " /// " << version->path << std::endl;
  }
  std::cout << "______________________________________" << std::endl;

  std::cout << "________________ LATEST ______________________" << std::endl;
  std::cout << ctx.latest_vortex_version->name << " /// " << ctx.latest_vortex_version->version << " /// "
            << ctx.latest_vortex_version->path << std::endl;
  std::cout << "______________________________________" << std::endl;
}
