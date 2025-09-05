//
// Created by hubin on 25-7-24.
//

#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <odb/mysql/database.hxx>
#include "pool/thread_pool.h"
#include "net/inet_address.h"
#include "net/acceptor.h"
#include "net/socket.h"


class EventLoop;
class Socket;
class InetAddress;
class Acceptor;
class Connection;
template<typename T>
class ThreadPool;
class Router;
class UserRepository;
class UserHandlers;

class ChatServer {
public:
    ChatServer(const std::shared_ptr<odb::mysql::database> &db, const std::shared_ptr<EventLoop> &mainLoop);

    ~ChatServer() = default;

    void NewConnection(const std::shared_ptr<Socket> &);

    void DeleteConnection(int);

    void NewMysqlInsert();

private:
    std::shared_ptr<odb::mysql::database> db;
    std::shared_ptr<EventLoop> mainLoop;
    std::shared_ptr<InetAddress> serverAddr;
    std::unique_ptr<Acceptor> acceptor;
    std::shared_ptr<Router> router;
    std::shared_ptr<UserRepository> ud;
    std::map<int, std::shared_ptr<Connection> > connections;
    std::unique_ptr<ThreadPool<std::function<void()> > > threadPool;
    std::vector<std::shared_ptr<EventLoop> > subReactors;
};

#endif //CHAT_SERVER_H
