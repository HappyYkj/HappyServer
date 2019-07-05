#ifndef _MYSQL_MANAGER_H
#define _MYSQL_MANAGER_H

#include <vector>
#include <functional>
#include <mysql.h>

class CMysqlManager
{
public:
    CMysqlManager();
    ~CMysqlManager();

public:
    bool connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);
    bool execute(const char* cmd);
    bool command(const char* cmd, std::function<void(MYSQL_RES* res)> func);

private:
    MYSQL mysql; //一个数据库结构体
};

#endif // !_MYSQL_MANAGER_H
