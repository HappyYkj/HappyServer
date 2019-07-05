#include "MysqlManager.h"
#include <stdio.h>

CMysqlManager::CMysqlManager()
{
    // ��ʼ�����ݿ�
    mysql_init(&mysql);

    // ���ñ��뷽ʽ
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");
}

CMysqlManager::~CMysqlManager()
{
    // �ر����ݿ�
    mysql_close(&mysql);
}

bool CMysqlManager::connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
    // �������ݿ�
    if (mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0) == NULL) {
        // ����ʧ��
        return false;
    }

    return true;
}

bool CMysqlManager::execute(const char* cmd)
{
    // ��ѯ����
    if (mysql_query(&mysql, cmd)) {
        // ִ��ʧ��
        return false;
    }
    return true;
}

bool CMysqlManager::command(const char* cmd, std::function<void(MYSQL_RES* res)> func)
{
    // ��ѯ����
    if (mysql_query(&mysql, cmd)) {
        // ִ��ʧ��
        return false;
    }

    // ��ȡ�����
    MYSQL_RES* res = mysql_store_result(&mysql); //һ��������ṹ��
    if (res) {
        // �ص������
        func(res);

        // �ͷŽ����
        mysql_free_result(res);
    }

    return true;
}
