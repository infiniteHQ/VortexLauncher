#include "../../../../lib/restcpp/include/restclient-cpp/restclient.h"
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

  RestClient::Response r = RestClient::get(url);

  if (r.code != 200) {
    return;
  }

  nlohmann::json jsonData = nlohmann::json::parse(r.body);

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
}

VORTEX_API void VortexMaker::UpdateVortexNews(const std::vector<std::string> &topics) {
  for (const auto &topic : topics) {
    std::string url = "http://api.infinite.si:9000/api/vortexupdates/get_news?topic=" + topic;

    std::cout << "Fetching URL: " << url << std::endl;

    RestClient::Response r = RestClient::get(url);

    if (r.code != 200) {
      std::cerr << "Failed to fetch news for topic: " << topic << std::endl;
      continue;
    }

    try {
      std::string responseBody = r.body;
      std::cout << "Response body: " << responseBody << std::endl;

      // Déséchapper la chaîne JSON pour pouvoir la parser
      responseBody = responseBody.substr(1, responseBody.length() - 2);  // Enlever les guillemets supplémentaires

      // Déséchapper les guillemets et autres caractères échappés
      std::string unescapedResponse = nlohmann::json::parse("\"" + responseBody + "\"").get<std::string>();

      // Maintenant, parser la chaîne déséchappée
      nlohmann::json jsonData = nlohmann::json::parse(unescapedResponse);

      // Créer et remplir un article
      VortexNews article;
      article.topic = topic;
      article.title = jsonData.value("title", "");              // Utilise une valeur par défaut si manquant
      article.description = jsonData.value("description", "");  // Utilise une valeur par défaut si manquant
      article.image_link = jsonData.value("image_link", "");    // Utilise une valeur par défaut si manquant
      article.news_link = jsonData.value("news_link", "");      // Utilise une valeur par défaut si manquant

      // Ajouter l'article au contexte
      VortexMaker::GetCurrentContext()->IO.news.push_back(article);
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

    RestClient::Response r = RestClient::get(url);

    if (r.code != 200) {
      VortexMaker::LogWarn("Failed to fetch data for dist", dist);
      continue;
    }

    nlohmann::json jsonData = nlohmann::json::parse(r.body);

    for (auto &item : jsonData) {
      item["dist"] = dist;
    }

    aggregatedJsonData.insert(aggregatedJsonData.end(), jsonData.begin(), jsonData.end());
  }

  std::shared_ptr<VortexVersion> latest_vortex_version;
  std::string highestVersion = "0.0.0";

  for (const auto &item : aggregatedJsonData) {
    std::shared_ptr<VortexVersion> version = std::make_shared<VortexVersion>();
    version->version = item["version"].get<std::string>();
    version->name = item["name"].get<std::string>();
    version->path = item["path"].get<std::string>();
    version->sum = item["sum"].get<std::string>();
    version->date = item["date"].get<std::string>();
    version->dist = item["dist"].get<std::string>();
    version->plat = plat;
    version->arch = arch;

    if (item.contains("values")) {
      auto values = nlohmann::json::parse(item["values"].get<std::string>());
      if (values.contains("image")) {
        version->banner = values["image"].get<std::string>();
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
