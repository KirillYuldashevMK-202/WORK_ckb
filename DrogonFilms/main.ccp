#include <drogon/HttpAppFramework.h>
#include "sqlite3.h"
#include <vector>
#include <string>
#include <iostream>

//функция обратного вызова
int СallbackSql(void* data, int argc, char** argv, char** azColName) {
    
    std::string* responseText = static_cast<std::string*>(data);
    for (int i = 0; i < argc; i++) {
        *responseText += azColName[i];
        *responseText += ": ";
        *responseText += argv[i] ? argv[i] : "NULL";
        *responseText += "<br>";
    }
    *responseText += "<hr>";
    return 0;
}

int main() {
    drogon::app().addListener("0.0.0.0", 1024);
    //CRUD
    //Read SQL
    drogon::app().registerHandler("/Read", [](const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            //Открытие базы данных
            sqlite3* db;
            sqlite3_open("C:/Users/Kirill/SqlliteDataBase/FilmsDataBase.db", &db);

            std::string responseText;
            //Запрос посылаемый базе данных
            const char* sql = "SELECT * from FilmsMain";
            char* ErrorSql = nullptr;
            int result = sqlite3_exec(db, sql, СallbackSql, &responseText, &ErrorSql);
            if (result != SQLITE_OK) {
                sqlite3_free(ErrorSql);
            }
            sqlite3_close(db);

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_TEXT_HTML);
            resp->setBody(responseText);
            callback(resp);
        });

    //Create SQL
    drogon::app().registerHandler("/Create", [](const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            //Обработка и парсинг запроса
            auto Fields = req->getParameters();
            std::string ID = Fields["id"];
            std::string Films = Fields["Films"];
            std::string Grade = Fields["Grade"];
            std::string Time = Fields["Time"];
            //Открытие базы данных
            sqlite3* db;
            sqlite3_open("C:/Users/Kirill/SqlliteDataBase/FilmsDataBase.db", &db);
            //Составление запроса для базы данных 
            std::string sql = "INSERT INTO FilmsMain (Id, Films, Grade, Time) VALUES ('" + ID + "','" + Films + "', '" + Grade + "', '" + Time + "')";
            char* ErrorSql = nullptr;
            int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &ErrorSql);
            if (result != SQLITE_OK) {
                sqlite3_free(ErrorSql);
            }
            sqlite3_close(db);

            auto resp = drogon::HttpResponse::newHttpResponse();
            callback(resp);
        });

    //Update SQL
    drogon::app().registerHandler("/Update", [](const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            //Обработка и парсинг запроса
            auto Fields = req->getParameters();
            std::string ID = Fields["id"];
            std::string Films = Fields["Films"];
            std::string Grade = Fields["Grade"];
            std::string Time = Fields["Time"];
            //Открытие базы данных
            sqlite3* db;
            sqlite3_open("C:/Users/Kirill/SqlliteDataBase/FilmsDataBase.db", &db);
            //Составление запроса для базы данных 
            std::string sql = "UPDATE FilmsMain SET Films = '" + Films + "', Grade = '" + Grade + "', Time = '" + Time + "' WHERE id = '" + ID + "'";
            char* ErrorSql = nullptr;
            int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &ErrorSql);
            if (result != SQLITE_OK) {
                sqlite3_free(ErrorSql);
            }
            sqlite3_close(db);
            auto resp = drogon::HttpResponse::newHttpResponse();
            callback(resp);
        });

    //Delete SQL
    drogon::app().registerHandler("/Delete", [](const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            auto Fields = req->getParameters();
            std::string ID = Fields["id"];
            //Открытие базы данных
            sqlite3* db;
            sqlite3_open("C:/Users/Kirill/SqlliteDataBase/FilmsDataBase.db", &db);
            //Составление запроса для базы данных 
            std::string sql = "DELETE FROM FilmsMain WHERE id = '" + ID + "'";
            char* ErrorSql = nullptr;
            int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &ErrorSql);
            if (result != SQLITE_OK) {
                sqlite3_free(ErrorSql);
            }
            sqlite3_close(db);
            auto resp = drogon::HttpResponse::newHttpResponse();
            callback(resp);
        });

    drogon::app().run();
    return 0;
}
