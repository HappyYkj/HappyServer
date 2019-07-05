#include "MysqlManager.h"
#include <stdio.h>

CMysqlManager::CMysqlManager()
{
    // 初始化数据库
    mysql_init(&mysql);

    // 设置编码方式
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");
}

CMysqlManager::~CMysqlManager()
{
    // 关闭数据库
    mysql_close(&mysql);
}

bool CMysqlManager::connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
    // 连接数据库
    if (mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0) == NULL) {
        // 连接失败
        return false;
    }

    return true;
}

bool CMysqlManager::execute(const char* cmd)
{
    // 查询数据
    if (mysql_query(&mysql, cmd)) {
        // 执行失败
        return false;
    }
    return true;
}

bool CMysqlManager::command(const char* cmd, std::function<void(MYSQL_RES* res)> func)
{
    // 查询数据
    if (mysql_query(&mysql, cmd)) {
        // 执行失败
        return false;
    }

    // 获取结果集
    MYSQL_RES* res = mysql_store_result(&mysql); //一个结果集结构体
    if (res) {
        // 回调结果集
        func(res);

        // 释放结果集
        mysql_free_result(res);
    }

    return true;
}
