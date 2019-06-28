#ifndef _GAME_LOGGER_H
#define _GAME_LOGGER_H

#define SPDLOG_ACTIVE_LEVEL 0

#pragma warning(disable:4244)
#include "spdlog/spdlog.h"
#pragma warning(default:4244)

#define LOG_TRACE                   SPDLOG_TRACE
#define LOG_DEBUG                   SPDLOG_DEBUG
#define LOG_INFO                    SPDLOG_INFO
#define LOG_WARN                    SPDLOG_WARN
#define LOG_ERROR                   SPDLOG_ERROR
#define LOG_CRITICAL                SPDLOG_CRITICAL

#define H_TRACE(tag, ...)           SPDLOG_TRACE(__VA_ARGS__)
#define H_DEBUG(tag, ...)           SPDLOG_DEBUG(__VA_ARGS__)
#define H_INFO(tag, ...)            SPDLOG_INFO(__VA_ARGS__)
#define H_WARN(tag, ...)            SPDLOG_WARN(__VA_ARGS__)
#define H_ERROR(tag, ...)           SPDLOG_ERROR(__VA_ARGS__)
#define H_CRITICAL(tag, ...)        SPDLOG_CRITICAL(__VA_ARGS__)

void LoggerInit();
void LoggerFini();

#endif // !_GAME_LOGGER_H
