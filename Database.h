#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mysql.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core/types_c.h>

namespace MySql {
    class Database {
    public:
        MYSQL* conn;

        Database() {
            conn = mysql_init(NULL);
            if (conn == NULL) {
                std::cout << "mysql_init() failed\n";
                exit(1);
            }

            // MySQL 서버에 연결
            if (mysql_real_connect(conn, "localhost", "root", "1234", "open_cv", 3306, NULL, 0) == NULL) {
                std::cout << "mysql_real_connect() failed\n";
                mysql_close(conn);
                exit(1);
            }
        }

        ~Database() {
            if (conn) {
                mysql_close(conn);
            }
        }

        // 데이터베이스에 데이터를 삽입하는 함수
        void InsertData(double width, double height, int lensCount, const std::string& color, const std::vector<uchar>& imageBuffer) {
            std::string query = "INSERT INTO phone (width, height, lensNum, color, image) VALUES (" +
                std::to_string(width) + ", " + std::to_string(height) + ", " +
                std::to_string(lensCount) + ", '" + color + "', ?)";

            MYSQL_STMT* stmt = mysql_stmt_init(conn);
            if (!stmt) {
                std::cerr << "mysql_stmt_init() failed\n";
                return;
            }

            if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
                std::cerr << "mysql_stmt_prepare() failed: " << mysql_stmt_error(stmt) << "\n";
                mysql_stmt_close(stmt);
                return;
            }

            MYSQL_BIND bind[1];
            memset(bind, 0, sizeof(bind));

            // BLOB 바인딩
            bind[0].buffer_type = MYSQL_TYPE_BLOB;
            bind[0].buffer = (char*)imageBuffer.data();
            bind[0].buffer_length = imageBuffer.size();

            if (mysql_stmt_bind_param(stmt, bind)) {
                std::cerr << "mysql_stmt_bind_param() failed: " << mysql_stmt_error(stmt) << "\n";
                mysql_stmt_close(stmt);
                return;
            }

            if (mysql_stmt_execute(stmt)) {
                std::cerr << "mysql_stmt_execute() failed: " << mysql_stmt_error(stmt) << "\n";
            }
            else {
                std::cout << "Data inserted successfully.\n";
            }

            mysql_stmt_close(stmt);
        }
    };
}