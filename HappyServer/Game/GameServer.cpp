#include "GameServer.h"
#include "GameLogger.h"

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
}

bool CGameServer::Start()
{
    // 初始日志
    LoggerInit();

    spdlog::info("Welcome to Happy Server version {}.{}.{} !", HAPPY_VER_MAJOR, HAPPY_VER_MINOR, HAPPY_VER_PATCH);
    return true;
}

void CGameServer::Stop()
{
    // 销毁日志
    LoggerFini();
}
