#include "GameLogger.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void LoggerInit()
{
    // Create multi threaded logger.
    //WL::FILE::PATH::createDirectoryIfNotExist(WL::FILE::PATH::execPathCatFileName("logs"));
    //auto file_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(WL::FILE::PATH::execPathCatFileName("logs/daily.txt"), 0, 0);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // Customize msg format for all loggers
    spdlog::set_default_logger(std::shared_ptr<spdlog::logger>(new spdlog::logger("game_logger", { /*file_logger, */console_sink })));
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%L][thread %t]%@: %^%v%$");

    // spdlog::info("Welcome to spdlog version {}.{}.{} !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
}

void LoggerFini()
{
    // Release all spdlog resources, and drop all loggers in the registry.
    // This is optional (only mandatory if using windows + async log).
    spdlog::shutdown();
}
