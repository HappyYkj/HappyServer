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
    // ��ʼ��־
    LoggerInit();

    spdlog::info("Welcome to Happy Server version {}.{}.{} !", HAPPY_VER_MAJOR, HAPPY_VER_MINOR, HAPPY_VER_PATCH);
    return true;
}

void CGameServer::Stop()
{
    // ������־
    LoggerFini();
}
