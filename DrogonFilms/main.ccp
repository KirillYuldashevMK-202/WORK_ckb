#include <drogon/HttpAppFramework.h>
#include "sqlite3.h"
#include <vector>
#include <string>

std::vector<std::string> Data;

static int callback(void* data, int argc, char** argv, char** azColName) {
    std::string row;
    for (int i = 0; i < argc; i++) {
        row += azColName[i];
        row += ": ";
        row += argv[i] ? argv[i] : "NULL";
        row += "<br>";
    }
    row += "<hr>";
    Data.push_back(row);
    return 0;
}

int main() {
    drogon::app().addListener("0.0.0.0", 1024);

    sqlite3* db;
    int result = sqlite3_open("C:/Users/Kirill/SqlliteDataBase/FilmsDataBase.db", &db);
    if (result) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    const char* sql = "SELECT * from FilmsMain";
    char* ErrorSql = nullptr;
    result = sqlite3_exec(db, sql, callback, nullptr, &ErrorSql);
    if (result != SQLITE_OK) {
        std::cout << "SQL error: " << ErrorSql << std::endl;
        sqlite3_free(ErrorSql);
    }
    sqlite3_close(db);

    drogon::app().registerHandler("/", [](const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_TEXT_HTML);

            std::string body;
            for (const auto& row : Data) {
                body += row;
            }
            resp->setBody(body);

            callback(resp);
        });

    drogon::app().run();
    return 0;
}
