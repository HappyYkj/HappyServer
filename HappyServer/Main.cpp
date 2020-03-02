#include <mutex>
#include <Windows.h>
#include <filesystem>
#include "GameScript.h"
#pragma comment(lib, "lua.lib")

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

void usage(void)
{
    printf("Usage:\n");
    printf("        Happy [-f lua-filename]\n");
    printf("The options are:\n");
    printf("        -f          run a lua file. will change current working directory to lua file's path.\n");
    printf("Examples:\n");
    printf("        Happy -f aoi_example.lua\n");
}

int main(int argc, char* argv[])
{
    system("chcp 65001");

    std::string startup_file = "main.lua";   // default startup
    for (int i = 1; i < argc; ++i)
    {
        bool lastarg = i == (argc - 1);
        std::string_view v{ argv[i] };
        if ((v == "-h" || v == "--help") && !lastarg)
        {
            usage();
            return 0;
        }
        else if ((v == "-f" || v == "--file") && !lastarg)
        {
            startup_file = argv[++i];
            if (std::filesystem::path(startup_file).extension().generic_string() != ".lua")
            {
                printf("service file must be a lua script.\n");
                usage();
                return 0;
            }

            if (!std::filesystem::exists(startup_file))
            {
                printf("startup file not exits.\n");
                usage();
                return 0;
            }
        }
        else
        {
            usage();
            return 0;
        }
    }

    // �󶨼���
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        return -1;
    }

    GameScript::instance().start(startup_file);

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
