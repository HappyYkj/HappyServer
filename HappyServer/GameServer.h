#ifndef _GAME_SERVER_H
#define _GAME_SERVER_H
#include <lua.hpp>
#include <sol/sol.hpp>
#include "Singleton.h"

#define HAPPY_VER_MAJOR 0
#define HAPPY_VER_MINOR 1
#define HAPPY_VER_PATCH 0

class CGameServer
{
public:
    CGameServer();
    ~CGameServer();

public:
    bool RunLoop();

private:
    sol::state lua;
};

typedef Container::singleton_default<CGameServer> GameServer;

#endif // !_GAME_SERVER_H
