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
    // ��ʼ��־
    LoggerInit();

    // ����汾
    spdlog::info("Welcome to Happy Server version {}.{}.{} !", HAPPY_VER_MAJOR, HAPPY_VER_MINOR, HAPPY_VER_PATCH);

    // �����ű�
    GameScript::instance().Run();

    // ������־
    LoggerFini();
    return true;
}
