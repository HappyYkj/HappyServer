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
    // 绑定监听
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        return -1;
    }

    GameScript::instance().start();

    // 等待事件
    std::unique_lock<std::mutex> lock(g_objExitMutex);
    g_objExitCond.wait(lock);

    GameScript::instance().stop();

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
