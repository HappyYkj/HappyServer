#ifndef _GAME_SCRIPT_H
#define _GAME_SCRIPT_H
#include "Singleton.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

/* Lua 5.4 does not have the updated
* error codes for thread status/function returns (but some patched versions do)
* define it only if it's not found
*/
#if !defined(LUA_ERRGCMM)
/* Use + 2 because in some versions of Lua (Lua 5.4)
* LUA_ERRFILE is defined as (LUA_ERRERR+1)
* so we need to avoid it (LuaJIT might have something at this
* integer value too)
*/
#  define LUA_ERRGCMM (LUA_ERRERR + 2)
#endif /* LUA_ERRGCMM define */

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
