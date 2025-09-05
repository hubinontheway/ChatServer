//
// Created by hubin on 25-7-20.
//
#include <memory>
#include <iostream>
#include <cstdlib> // 为了使用 std::getenv
#include <odb/mysql/database.hxx>
#include <odb/session.hxx>
#include <server/chat_server.h>
#include "net/event_loop.h"

int main(int argc, char *argv[]) {
    // 从环境变量获取数据库连接参数
    const char *user = std::getenv("MYSQL_USER");
    const char *password = std::getenv("MYSQL_PASSWORD");
    const char *database = std::getenv("MYSQL_DATABASE");
    const char *host = std::getenv("MYSQL_HOST");
    const char *port = std::getenv("MYSQL_PORT");

    std::shared_ptr<odb::mysql::database> db;
    try {
        // 使用环境变量的值创建数据库对象
        db = std::make_shared<odb::mysql::database>(
            user,
            password,
            database,
            host,
            std::stoi(port)
        );

        odb::session s;
        odb::transaction t(db->begin());
        t.commit();
        std::cout << "数据库连接成功" << std::endl;
    } catch (const odb::exception &e) {
        std::cerr << "数据库连接失败：" << e.what() << std::endl;
        return 1;
    }

    auto mainLoop = std::make_shared<EventLoop>(0);
    auto server = std::make_shared<ChatServer>(db, mainLoop);
    mainLoop->Loop();

    return 0;
}
