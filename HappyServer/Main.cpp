#include <mutex>
#include <Windows.h>
#include "GameScript.h"
#pragma comment(lib, "lua.lib")
#pragma comment(lib, "lfs.lib")
#pragma comment(lib, "lanes.lib")

std::mutex g_objExitMutex;
std::condition_variable g_objExitCond;

BOOL WINAPI ConsoleHandler(DWORD msgType)
{
    if (msgType == CTRL_C_EVENT ||
        msgType == CTRL_CLOSE_EVENT ||
        msgType == CTRL_BREAK_EVENT ||
        msgType == CTRL_LOGOFF_EVENT ||
        msgType == CTRL_SHUTDOWN_EVENT) {
        g_objExitCond.notify_one();
        return TRUE;
    }
    return FALSE;
}

int main()
{
    // �󶨼���
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        return -1;
    }

    GameScript::instance().start();

    // �ȴ��¼�
    std::unique_lock<std::mutex> lock(g_objExitMutex);
    g_objExitCond.wait(lock);

    GameScript::instance().stop();

    return 0;
}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ������ʾ: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
