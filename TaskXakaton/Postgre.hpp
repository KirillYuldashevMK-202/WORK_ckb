#ifndef MODULE_POSTGRE
#define MODULE_POSTGRE

#include <iostream>
#include <libpq-fe.h>
#include <string>
#include <vector>

class WorkDB {
public:
    std::string ErrorMsg;

    WorkDB(std::string host, std::string port, std::string dbname, std::string user, std::string pass);

    int RequestSQL(std::string request);

    int CheckFilesUsers(std::string idUsers, std::string nameFiles);

    int AddFilesUsers(std::string idUsers, std::string nameFiles);

    int DeleteFilesUsers(std::string idUsers, std::string nameFiles);

    int DeleteUser(std::string idUsers);

    std::vector<std::string> GetDB();

    ~WorkDB();

private:
    PGconn* conn;
    PGresult* res;
};

#endif