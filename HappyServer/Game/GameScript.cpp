#include "GameScript.h"
#include "GameLogger.h"
#include <signal.h>

#if !defined(LUA_PROGNAME)
#define LUA_PROGNAME        "lua"
#endif

static lua_State *globalL = NULL;

static const char *progname = LUA_PROGNAME;


/*
** Hook set by signal function to stop the interpreter.
*/
static void lstop(lua_State *L, lua_Debug *ar) {
    (void)ar;  /* unused arg. */
    lua_sethook(L, NULL, 0, 0);  /* reset hook */
    luaL_error(L, "interrupted!");
}


/*
** Function to be called at a C signal. Because a C signal cannot
** just change a Lua state (as there is no proper synchronization),
** this function only sets a hook that, when called, will stop the
** interpreter.
*/
static void laction(int i) {
    signal(i, SIG_DFL); /* if another SIGINT happens, terminate process */
    lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}


/*
** Prints an error message, adding the program name in front of it
** (if present)
*/
static void l_message(const char *pname, const char *msg) {
    if (pname) lua_writestringerror("%s: ", pname);
    lua_writestringerror("%s\n", msg);
}


/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack. It assumes that the error object
** is a string, as it was either generated by Lua or by 'msghandler'.
*/
static int report(lua_State *L, int status) {
    if (status != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        l_message(progname, msg);
        lua_pop(L, 1);  /* remove message */
    }
    return status;
}


/*
** Message handler used to run all chunks
*/
static int msghandler(lua_State *L) {
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL) {  /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
            lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
            return 1;  /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)",
                luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
    return 1;  /* return the traceback */
}


/*
** Interface to 'lua_pcall', which sets appropriate message function
** and C-signal handler. Used to run all chunks.
*/
static int docall(lua_State *L, int narg, int nres) {
    int status;
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushcfunction(L, msghandler);  /* push message handler */
    lua_insert(L, base);  /* put it under function and args */
    globalL = L;  /* to be available to 'laction' */
    signal(SIGINT, laction);  /* set C-signal handler */
    status = lua_pcall(L, narg, nres, base);
    signal(SIGINT, SIG_DFL); /* reset C-signal handler */
    lua_remove(L, base);  /* remove message handler from the stack */
    return status;
}

/*
** ===================================================================
** Read-Eval-Print Loop (REPL)
** ===================================================================
*/

#if !defined(LUA_PROMPT)
#define LUA_PROMPT		"> "
#define LUA_PROMPT2		">> "
#endif

#if !defined(LUA_MAXINPUT)
#define LUA_MAXINPUT		512
#endif


/*
** lua_stdin_is_tty detects whether the standard input is a 'tty' (that
** is, whether we're running lua interactively).
*/
#if !defined(lua_stdin_is_tty)	/* { */

#if defined(LUA_USE_POSIX)	/* { */

#include <unistd.h>
#define lua_stdin_is_tty()	isatty(0)

#elif defined(LUA_USE_WINDOWS)	/* }{ */

#include <io.h>
#include <windows.h>

#define lua_stdin_is_tty()	_isatty(_fileno(stdin))

#else				/* }{ */

/* ISO C definition */
#define lua_stdin_is_tty()	1  /* assume stdin is a tty */

#endif				/* } */

#endif				/* } */


/*
** lua_readline defines how to show a prompt and then read a line from
** the standard input.
** lua_saveline defines how to "save" a read line in a "history".
** lua_freeline defines how to free a line read by lua_readline.
*/
#if !defined(lua_readline)	/* { */

#if defined(LUA_USE_READLINE)	/* { */

#include <readline/readline.h>
#include <readline/history.h>
#define lua_initreadline(L)	((void)L, rl_readline_name="lua")
#define lua_readline(L,b,p)	((void)L, ((b)=readline(p)) != NULL)
#define lua_saveline(L,line)	((void)L, add_history(line))
#define lua_freeline(L,b)	((void)L, free(b))

#else				/* }{ */

#define lua_initreadline(L)  ((void)L)
#define lua_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        fgets(b, LUA_MAXINPUT, stdin) != NULL)  /* get line */
#define lua_saveline(L,line)	{ (void)L; (void)line; }
#define lua_freeline(L,b)	{ (void)L; (void)b; }

#endif				/* } */

#endif				/* } */


/*
** Returns the string to be used as a prompt by the interpreter.
*/
static const char *get_prompt(lua_State *L, int firstline) {
    const char *p;
    lua_getglobal(L, firstline ? "_PROMPT" : "_PROMPT2");
    p = lua_tostring(L, -1);
    if (p == NULL) p = (firstline ? LUA_PROMPT : LUA_PROMPT2);
    return p;
}

/* mark in error messages for incomplete statements */
#define EOFMARK		"<eof>"
#define marklen		(sizeof(EOFMARK)/sizeof(char) - 1)


/*
** Check whether 'status' signals a syntax error and the error
** message at the top of the stack ends with the above mark for
** incomplete statements.
*/
static int incomplete(lua_State *L, int status) {
    if (status == LUA_ERRSYNTAX) {
        size_t lmsg;
        const char *msg = lua_tolstring(L, -1, &lmsg);
        if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
            lua_pop(L, 1);
            return 1;
        }
    }
    return 0;  /* else... */
}


/*
** Prompt the user, read a line, and push it into the Lua stack.
*/
static int pushline(lua_State *L, int firstline) {
    char buffer[LUA_MAXINPUT];
    char *b = buffer;
    size_t l;
    const char *prmt = get_prompt(L, firstline);
    int readstatus = lua_readline(L, b, prmt);
    if (readstatus == 0)
        return 0;  /* no input (prompt will be popped by caller) */
    lua_pop(L, 1);  /* remove prompt */
    l = strlen(b);
    if (l > 0 && b[l - 1] == '\n')  /* line ends with newline? */
        b[--l] = '\0';  /* remove it */
    if (firstline && b[0] == '=')  /* for compatibility with 5.2, ... */
        lua_pushfstring(L, "return %s", b + 1);  /* change '=' to 'return' */
    else
        lua_pushlstring(L, b, l);
    lua_freeline(L, b);
    return 1;
}


/*
** Try to compile line on the stack as 'return <line>;'; on return, stack
** has either compiled chunk or original line (if compilation failed).
*/
static int addreturn(lua_State *L) {
    const char *line = lua_tostring(L, -1);  /* original line */
    const char *retline = lua_pushfstring(L, "return %s;", line);
    int status = luaL_loadbuffer(L, retline, strlen(retline), "=stdin");
    if (status == LUA_OK) {
        lua_remove(L, -2);  /* remove modified line */
        if (line[0] != '\0')  /* non empty? */
            lua_saveline(L, line);  /* keep history */
    }
    else
        lua_pop(L, 2);  /* pop result from 'luaL_loadbuffer' and modified line */
    return status;
}


/*
** Read multiple lines until a complete Lua statement
*/
static int multiline(lua_State *L) {
    for (;;) {  /* repeat until gets a complete statement */
        size_t len;
        const char *line = lua_tolstring(L, 1, &len);  /* get what it has */
        int status = luaL_loadbuffer(L, line, len, "=stdin");  /* try it */
        if (!incomplete(L, status) || !pushline(L, 0)) {
            lua_saveline(L, line);  /* keep history */
            return status;  /* cannot or should not try to add continuation line */
        }
        lua_pushliteral(L, "\n");  /* add newline... */
        lua_insert(L, -2);  /* ...between the two lines */
        lua_concat(L, 3);  /* join them */
    }
}


/*
** Read a line and try to load (compile) it first as an expression (by
** adding "return " in front of it) and second as a statement. Return
** the final status of load/call with the resulting function (if any)
** in the top of the stack.
*/
static int loadline(lua_State *L) {
    int status;
    lua_settop(L, 0);
    if (!pushline(L, 1))
        return -1;  /* no input */
    if ((status = addreturn(L)) != LUA_OK)  /* 'return ...' did not work? */
        status = multiline(L);  /* try as command, maybe with continuation lines */
    lua_remove(L, 1);  /* remove line from the stack */
    lua_assert(lua_gettop(L) == 1);
    return status;
}


/*
** Prints (calling the Lua 'print' function) any values on the stack
*/
static void l_print(lua_State *L) {
    int n = lua_gettop(L);
    if (n > 0) {  /* any result to be printed? */
        luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
        lua_getglobal(L, "print");
        lua_insert(L, 1);
        if (lua_pcall(L, n, 0, 0) != LUA_OK)
            l_message(progname, lua_pushfstring(L, "error calling 'print' (%s)",
                lua_tostring(L, -1)));
    }
}


/*
** Do the REPL: repeatedly read (load) a line, evaluate (call) it, and
** print any results.
*/
static void doREPL(lua_State *L) {
    int status;
    lua_initreadline(L);
    while ((status = loadline(L)) != -1) {
        if (status == LUA_OK)
            status = docall(L, 0, LUA_MULTRET);
        if (status == LUA_OK) l_print(L);
        else report(L, status);
    }
    lua_settop(L, 0);  /* clear stack */
    lua_writeline();
}

/*
** ===================================================================
** Game Lua Script Proxy
** ===================================================================
*/

#include "RedisManager.h"

CGameScript::CGameScript()
{
}

CGameScript::~CGameScript()
{
}

bool CGameScript::Run()
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

    // 设置路径
    std::string path;
    path.append("Script/?.lua;").append("Script/?/init.lua;").append(lua["package"]["path"]);
    lua["package"]["path"] = path;

    RedisManager::instance().Start(lua);

    // 执行脚本
    sol::protected_function_result result = lua.script_file("Script/main.lua", sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        H_ERROR("script", "\n{}", err.what());
        return false;
    }

    doREPL(lua.lua_state());

    RedisManager::instance().Stop(lua);
    return true;
}
