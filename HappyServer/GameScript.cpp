#include "GameScript.h"

extern "C" {
#include "lfs.h"
#include "lanes.h"
}

/*
** ===================================================================
** Game Spd Logger Manager
** ===================================================================
*/

#pragma warning(disable:4244)
#include "spdlog/spdlog.h"
#pragma warning(default:4244)
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

static int luaopen_spdlog(sol::state& lua) {
    sol::table lua_spdlog = lua.create_named_table("spdlog");

    // Log level enum
    lua_spdlog["level"] = lua.create_table_with(
        "trace", SPDLOG_LEVEL_TRACE,
        "debug", SPDLOG_LEVEL_DEBUG,
        "info", SPDLOG_LEVEL_INFO,
        "warn", SPDLOG_LEVEL_WARN,
        "err", SPDLOG_LEVEL_ERROR,
        "critical", SPDLOG_LEVEL_CRITICAL,
        "off", SPDLOG_LEVEL_OFF
    );

    // Set global format string.
    lua_spdlog.set_function("set_pattern", [=](std::string pattern) {
        spdlog::set_pattern(pattern);
        });

    // Set global logging level
    lua_spdlog.set_function("set_level", spdlog::set_level);

    // Set global flush level
    lua_spdlog.set_function("flush_on", spdlog::flush_on);

    // Drop the reference to the given logger
    lua_spdlog.set_function("drop", spdlog::drop);

    // Drop all references from the registry
    lua_spdlog.set_function("drop_all", spdlog::drop_all);

    // stop any running threads started by spdlog and clean registry loggers
    lua_spdlog.set_function("shutdown", spdlog::shutdown);

    // Create and register a logger with a templated sink type
    // The logger's level, formatter and flush level will be set according the
    // global settings.
    lua_spdlog.set_function("create", [=](std::string logger_name, std::string filename) {
        spdlog::create<spdlog::sinks::daily_file_sink_st>(logger_name, filename, 0, 0);
        });

    // API for using default logger (stdout_color_mt),
    // e.g: spdlog::info("Message {}", 1);
    //
    // The default logger object can be accessed using the spdlog::default_logger():
    // For example, to add another sink to it:
    // spdlog::default_logger()->sinks()->push_back(some_sink);
    //
    // The default logger can replaced using spdlog::set_default_logger(new_logger).
    // For example, to replace it with a file logger.
    //
    // IMPORTANT:
    // The default API is thread safe (for _mt loggers), but:
    // set_default_logger() *should not* be used concurrently with the default API.
    // e.g do not call set_default_logger() from one thread while calling spdlog::info() from another.
    lua_spdlog.set_function("set_default_logger", [=](std::string tag, std::vector<std::string> sink_names) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // Customize msg format for all loggers
        spdlog::set_default_logger(std::shared_ptr<spdlog::logger>(new spdlog::logger(tag, { console_sink })));

        // Add another sink to default logger
        for (auto& sink_name : sink_names) {
            auto sink = spdlog::get(sink_name);
            if (!sink) {
                continue;
            }

            for (auto& sink_ptr : sink->sinks()) {
                spdlog::default_logger()->sinks().emplace_back(sink_ptr);
            }
        }
        });

    // trace
    lua_spdlog.set_function("trace", [=](std::string tag, std::string msg) {
        auto logger = spdlog::get(tag);
        if (!logger) {
            spdlog::trace(msg);
            return;
        }

        logger->trace(msg);
        });

    // debug
    lua_spdlog.set_function("debug", sol::overload(
        [=](std::string msg) {
            spdlog::debug(msg);
        },
        [=](std::string tag, std::string msg) {
            auto logger = spdlog::get(tag);
            if (!logger) {
                spdlog::debug(msg);
                return;
            }

            logger->debug(msg);
        }
        ));

    // info
    lua_spdlog.set_function("info", sol::overload(
        [=](std::string msg) {
            spdlog::info(msg);
        },
        [=](std::string tag, std::string msg) {
            auto logger = spdlog::get(tag);
            if (!logger) {
                spdlog::info(msg);
                return;
            }

            logger->info(msg);
        }
        ));

    // warn
    lua_spdlog.set_function("warn", sol::overload(
        [=](std::string msg) {
            spdlog::warn(msg);
        },
        [=](std::string tag, std::string msg) {
            auto logger = spdlog::get(tag);
            if (!logger) {
                spdlog::warn(msg);
                return;
            }

            logger->warn(msg);
        }
        ));

    // error
    lua_spdlog.set_function("error", sol::overload(
        [=](std::string msg) {
            spdlog::error(msg);
        },
        [=](std::string tag, std::string msg) {
            auto logger = spdlog::get(tag);
            if (!logger) {
                spdlog::error(msg);
                return;
            }

            logger->error(msg);
        }
        ));

    // critical
    lua_spdlog.set_function("critical", sol::overload(
        [=](std::string msg) {
            spdlog::critical(msg);
        },
        [=](std::string tag, std::string msg) {
            auto logger = spdlog::get(tag);
            if (!logger) {
                spdlog::critical(msg);
                return;
            }

            logger->critical(msg);
        }
        ));

    spdlog::set_default_logger(spdlog::stdout_color_mt("console"));
    spdlog::set_automatic_registration(true);
    return 1;
}

/*
** ===================================================================
**
** ===================================================================
*/

CGameScript::CGameScript()
    : m_event_stop(false)
    , m_event_thread(nullptr)
{
}

CGameScript::~CGameScript()
{
}

bool CGameScript::start()
{
    if (m_event_thread) {
        return true;
    }

    // 重置事件结束标记
    m_event_stop = false;

    // 创建事件处理线程
    m_event_thread = new std::thread(std::bind(&CGameScript::daemon, this));
    if (!m_event_thread) {
        return false;
    }

    return true;
}

void CGameScript::stop()
{
    if (!m_event_thread) {
        return;
    }

    // 通知线程结束
    m_event_stop = true;

    // 等待线程结束
    if (m_event_thread->joinable()) {
        m_event_thread->join();
    }

    // 释放线程资源
    delete m_event_thread;
    m_event_thread = nullptr;
}

void CGameScript::daemon()
{
    // 加载库
    lua.open_libraries(
        sol::lib::base,         // print, assert, and other base functions
        sol::lib::package,      // require and other package functions
        sol::lib::coroutine,    // coroutine functions and utilities
        sol::lib::string,       // string library
        sol::lib::os,           // functionality from the OS
        sol::lib::math,         // all things math
        sol::lib::table,        // the table manipulator and observer functions
        sol::lib::debug,        // the debug library
        sol::lib::bit32,        // the bit library: different based on which you're using
        sol::lib::io            // input/output library
    );

    // 嵌套第三方库
    lua.require("lfs", luaopen_lfs);
    lua.require("lanes.core", luaopen_lanes_core);

    // 设置路径
    std::string path;
    path.append(".\\?.lua;")
        .append(".\\?\\init.lua;")
        .append(".\\libs\\?.lua;")
        .append(".\\libs\\?\\init.lua;")
        .append(".\\src\\?.lua;")
        .append(".\\src\\?\\init.lua;")
        .append(lua["package"]["path"]);
    lua["package"]["path"] = path;

    std::string cpath;
    cpath.append(".\\clibs\\?.dll;")
        .append(lua["package"]["cpath"]);
    lua["package"]["cpath"] = cpath;

    // 日志模块
    luaopen_spdlog(lua);

    // 执行脚本
    sol::protected_function_result result = lua.script_file("main.lua", sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        spdlog::error("\n{}", err.what());
    }
}
