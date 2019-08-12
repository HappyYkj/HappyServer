#ifndef _GAME_SCRIPT_H
#define _GAME_SCRIPT_H
#include <thread>
#include <lua.hpp>
#include <sol/sol.hpp>
#include "Singleton.h"

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
    // �¼������߳�
    void daemon();

private:
    sol::state lua;

    // �¼��߳�ID
    std::thread* m_event_thread;

    // �¼��������
    bool m_event_stop;
};

typedef Container::singleton_default<CGameScript> GameScript;

#endif // !_GAME_SCRIPT_H
