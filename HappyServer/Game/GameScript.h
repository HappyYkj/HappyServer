#ifndef _GAME_SCRIPT_H
#define _GAME_SCRIPT_H
#include "Singleton.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <sol/sol.hpp>

class CGameScript
{
public:
    CGameScript();
    ~CGameScript();

public:
    bool Run();

private:
    sol::state lua;
};

typedef Container::singleton_default<CGameScript> GameScript;

#endif // !_GAME_SCRIPT_H
