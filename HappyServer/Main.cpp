#include <Windows.h>
#include "GameServer.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "lua.lib")

BOOL WINAPI ConsoleHandler(DWORD msgType)
{
    if (msgType == CTRL_C_EVENT ||
        msgType == CTRL_CLOSE_EVENT ||
        msgType == CTRL_BREAK_EVENT ||
        msgType == CTRL_LOGOFF_EVENT ||
        msgType == CTRL_SHUTDOWN_EVENT) {
        return TRUE;
    }
    return FALSE;
}

// 加载socket动态链接库
bool LoadWindowsSocketLib()
{
    // 请求2.2版本的WinSock库
    WORD wVersionRequested = MAKEWORD(2, 2);

    // 接收Windows Socket的结构信息
    WSADATA wsaData;

    // 检查套接字库是否申请成功
    DWORD err = WSAStartup(wVersionRequested, &wsaData);
    if (0 != err) {
        return false;
    }

    // 检查是否申请了所需版本的套接字库
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return false;
    }
    return true;
}

// 释放socket动态链接库
void UnLoadWindowsSocketLib()
{
    WSACleanup();
}

int main()
{
    // 绑定监听
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        return -1;
    }
 
    // 加载socket动态链接库
    if (!LoadWindowsSocketLib()) {
        return -2;
    }

    // 运行服务
    GameServer::instance().RunLoop();

    // 释放socket动态链接库
    UnLoadWindowsSocketLib();

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
