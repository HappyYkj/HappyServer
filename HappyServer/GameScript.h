#ifndef _GAME_SCRIPT_H
#define _GAME_SCRIPT_H

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "Singleton.h"
#include <sol/sol.hpp>
#include <thread>

#define HAPPY_VER_MAJOR 0
#define HAPPY_VER_MINOR 1
#define HAPPY_VER_PATCH 0

class CGameScript
{
public:
    CGameScript();
    ~CGameScript();

public:
    bool start();
    void stop();

private:
    // 事件处理线程
    void daemon();

private:
    sol::state lua;

    // 事件线程ID
    std::thread* m_event_thread;

    // 事件结束标记
    bool m_event_stop;
};

typedef Container::singleton_default<CGameScript> GameScript;

#endif // !_GAME_SCRIPT_H
