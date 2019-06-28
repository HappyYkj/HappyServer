#include <Windows.h>
#include "GameServer.h"
#include "GameScript.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "lua.lib")
// #pragma comment(lib, "hiredis.lib")

// ����socket��̬���ӿ�
bool LoadWindowsSocketLib()
{
    // ����2.2�汾��WinSock��
    WORD wVersionRequested = MAKEWORD(2, 2);

    // ����Windows Socket�Ľṹ��Ϣ
    WSADATA wsaData;

    // ����׽��ֿ��Ƿ�����ɹ�
    DWORD err = WSAStartup(wVersionRequested, &wsaData);
    if (0 != err) {
        return false;
    }

    // ����Ƿ�����������汾���׽��ֿ�
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return false;
    }
    return true;
}

// �ͷ�socket��̬���ӿ�
void UnLoadWindowsSocketLib()
{
    WSACleanup();
}

int main()
{
    // ����socket��̬���ӿ�
    LoadWindowsSocketLib();

    // ��������
    GameServer::instance().Start();

    // �����ű�
    GameScript::instance().Run();

    // ֹͣ����
    GameServer::instance().Stop();

    // �ͷ�socket��̬���ӿ�
    UnLoadWindowsSocketLib();

    system("pause");
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
