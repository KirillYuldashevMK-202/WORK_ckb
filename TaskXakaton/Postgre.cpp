#include "Postgre.hpp"
#include "Macros.hpp"

WorkDB::WorkDB(std::string host, std::string port, std::string dbname, std::string user, std::string pass) {
    std::string Info = "host=" + host + " " + "port=" + port + " " + "dbname=" + dbname + " " + "user=" + user + " " + "password=" + pass;
    this->conn = PQconnectdb(Info.c_str());
}

int WorkDB::RequestSQL(std::string request) {
    if (PQstatus(this->conn) != CONNECTION_OK) {
        this->ErrorMsg = "Error: connectdb. " + (std::string)PQerrorMessage(this->conn) + ".\n";
        PQfinish(this->conn);
        return ERROR;
    }

    this->res = PQexec(this->conn, request.c_str());

    ExecStatusType status = PQresultStatus(this->res);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        this->ErrorMsg = "Error: request sql. " + (std::string)PQerrorMessage(this->conn) + ".\n";
        PQclear(this->res);
        return ERROR;
    }
    PQclear(res);
    return OK;
}

int WorkDB::CheckFilesUsers(std::string idUsers, std::string nameFiles) {
    if (PQstatus(this->conn) != CONNECTION_OK) {
        this->ErrorMsg = "Error: connectdb. " + (std::string)PQerrorMessage(this->conn) + ".\n";
        PQfinish(this->conn);
        return ERROR;
    }
    std::string Check = "SELECT COUNT(*) FROM datauserfiles WHERE user_id = " + idUsers + " AND file_name = '" + nameFiles + "';";
    this->res = PQexec(this->conn, Check.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        this->ErrorMsg = "Error: SELECT query failed. " + (std::string)PQerrorMessage(this->conn) + ".\n";
        PQclear(res);
        return ERROR;
    }

    int count = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    if (count > 0) {
        return USERSEXISTS;
    }
    else {
        return OK;
    }
}

int WorkDB::AddFilesUsers(std::string idUsers, std::string nameFiles) {
    if (CheckFilesUsers(idUsers, nameFiles) == OK) {
        if (RequestSQL("INSERT INTO datauserfiles (user_id, file_name) VALUES (" + idUsers + ", " + "'" + nameFiles + "');") == ERROR) {
            return ERROR;
        }
    }
    else {
        return USERSEXISTS;
    }

    return OK;
}

int WorkDB::DeleteFilesUsers(std::string idUsers, std::string nameFiles) {
    if (CheckFilesUsers(idUsers, nameFiles) == USERSEXISTS) {
        if (RequestSQL("DELETE FROM datauserfiles WHERE user_id = " + idUsers + " AND file_name = '" + nameFiles + "';") == ERROR) {
            return ERROR;
        }
    }
    else {
        return USERSNOTEXISTS;
    }
    return OK;
}

int WorkDB::DeleteUser(std::string idUsers) {
    std::string query = "DELETE FROM datauserfiles WHERE user_id = " + idUsers + ";";
    if (RequestSQL(query.c_str()) == ERROR) {
        return ERROR;
    }

    return OK; 
}


std::vector<std::string> WorkDB::GetDB() {
    std::vector<std::string> lists;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        this->ErrorMsg = "Error: connectdb. " + (std::string)PQerrorMessage(this->conn) + ".\n";
        PQfinish(this->conn);
        return lists;
    }

    std::string ListsSQL = "SELECT user_id, file_name FROM datauserfiles;";
    this->res = PQexec(this->conn, ListsSQL.c_str());

    if (PQresultStatus(this->res) != PGRES_TUPLES_OK) {
        this->ErrorMsg = "Error: get db. " + (std::string)PQerrorMessage(this->conn) + ".\n";
        PQclear(this->res);
        return lists;
    }

    int rows = PQntuples(this->res);
    for (int i = 0; i < rows; ++i) {
        std::string userId = PQgetvalue(this->res, i, 0);
        std::string fileName = PQgetvalue(this->res, i, 1);
        std::string MainStr = userId + " : " + fileName;
        lists.push_back(MainStr);
    }
    PQclear(this->res);
    return lists;
}


WorkDB::~WorkDB() {
    PQfinish(this->conn);
}


