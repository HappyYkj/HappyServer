#ifndef _GAME_SERVER_H
#define _GAME_SERVER_H
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
};

typedef Container::singleton_default<CGameServer> GameServer;

#endif // !_GAME_SERVER_H
