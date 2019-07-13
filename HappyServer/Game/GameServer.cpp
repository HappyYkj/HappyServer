#include "GameServer.h"
#include "GameLogger.h"
#include "GameScript.h"

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
}

bool CGameServer::RunLoop()
{
    // 初始日志
    LoggerInit();

    // 输出版本
    spdlog::info("Welcome to Happy Server version {}.{}.{} !", HAPPY_VER_MAJOR, HAPPY_VER_MINOR, HAPPY_VER_PATCH);

    // 启动脚本
    GameScript::instance().Run();

    // 销毁日志
    LoggerFini();
    return true;
}
