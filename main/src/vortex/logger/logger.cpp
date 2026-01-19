#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::DropLoggers() {
  VxContext &ctx = *CVortexMaker;
  ctx.pool_loggers.clear();
  ctx.global_logger = nullptr;
  ctx.console_logger = nullptr;
}

VORTEX_API std::shared_ptr<VxLogger> VortexMaker::CreateLogPool(const std::string &pool_name) {
  VxContext &ctx = *CVortexMaker;
  std::string dir = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + ctx.state.session_id + "/logs/";
  std::string file_path = dir + pool_name + ".log";

  auto new_logger = std::make_shared<VxLogger>(pool_name, file_path);
  ctx.pool_loggers.push_back({ pool_name, new_logger });

  return new_logger;
}

VORTEX_API std::shared_ptr<VxLogger> VortexMaker::CreateGlobalLogger() {
  VxContext &ctx = *CVortexMaker;
  std::string file_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + ctx.state.session_id + "/logs/global.log";
  ctx.global_logger = std::make_shared<VxLogger>("global", file_path);
  return ctx.global_logger;
}

VORTEX_API std::shared_ptr<VxLogger> VortexMaker::CreateConsoleLogger() {
  VxContext &ctx = *CVortexMaker;
  ctx.console_logger = std::make_shared<VxLogger>("console", "", true);
  return ctx.console_logger;
}

void DispatchLog(VxLogLevel level, const std::string &pool_name, const std::string &scope, const std::string &message) {
  VxContext &ctx = *CVortexMaker;
  if (!ctx.logger)
    return;

  std::string formatted_msg = "[" + scope + "] : " + message;

  if (ctx.console_logger) {
    ctx.console_logger->log(level, pool_name, formatted_msg);
  }

  if (ctx.global_logger) {
    ctx.global_logger->log(level, pool_name, formatted_msg);
  }

  if (pool_name != "global") {
    auto it = std::find_if(
        ctx.pool_loggers.begin(), ctx.pool_loggers.end(), [&](const std::pair<std::string, std::shared_ptr<VxLogger>> &p) {
          return p.first == pool_name;
        });

    if (it != ctx.pool_loggers.end()) {
      it->second->log(level, pool_name, formatted_msg);
    } else {
      auto new_pool = VortexMaker::CreateLogPool(pool_name);
      new_pool->log(level, pool_name, formatted_msg);
    }
  }

  if (ctx.logger_registering) {
    auto log_entry = std::make_shared<VxSystemLog>(level, scope, message);
    log_entry->m_timestamp = VortexMaker::getCurrentTimeStamp();
    ctx.registered_logs.push_back(log_entry);
  }
}

VORTEX_API void VortexMaker::LogInfo(const std::string &pool_name, const std::string &scope, const std::string &message) {
  DispatchLog(VxLogLevel::info, pool_name, scope, message);
}

VORTEX_API void VortexMaker::LogInfo(const std::string &scope, const std::string &message) {
  DispatchLog(VxLogLevel::info, "global", scope, message);
}

VORTEX_API void VortexMaker::LogWarn(const std::string &scope, const std::string &message) {
  DispatchLog(VxLogLevel::warn, "global", scope, message);
}

VORTEX_API void VortexMaker::LogError(const std::string &scope, const std::string &message) {
  DispatchLog(VxLogLevel::err, "global", scope, message);
}

VORTEX_API void VortexMaker::LogFatal(const std::string &scope, const std::string &message) {
  DispatchLog(VxLogLevel::critical, "global", scope, message);
}