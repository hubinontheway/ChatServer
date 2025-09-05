//
// Created by hubin on 25-7-24.
//
#include "server/chat_server.h"
#include "repository/user_repository.h"
#include <utility>
#include "net/event_loop.h"
#include "http/router.h"
#include "net/connection.h"
#include "web/chat_handlers.h"
#include "web/user_handlers.h"

ChatServer::ChatServer(const std::shared_ptr<odb::mysql::database> &db, const std::shared_ptr<EventLoop> &mainLoop) {
    acceptor = make_unique<Acceptor>(mainLoop);
    router = make_shared<Router>();

    auto userHandlers = make_shared<UserHandlers>(router);
    auto chatHandlers = make_shared<ChatHandlers>(router);

    ud = make_shared<UserRepository>(db);
    const function<void(shared_ptr<Socket>)> cb = [this](const shared_ptr<Socket> &serverSocket) {
        this->NewConnection(serverSocket);
    };
    acceptor->SetNewConnectionCallBack(cb);

    int size = 2;
    threadPool = make_unique<ThreadPool<function<void()> > >(size, 1000);
    subReactors.reserve(size);
    for (int i = 0; i < size; i++) {
        subReactors.emplace_back(make_shared<EventLoop>(i + 1));
        threadPool->Append([this, i]() {
            subReactors[i]->Loop();
        });
    }
}

void ChatServer::NewConnection(const std::shared_ptr<Socket> &socket) {
    if (socket->GetFd() < 0) {
        cout << "NewConnection socket->GetFd() Err" << endl;
    }
    const int random = socket->GetFd() % subReactors.size();
    // 在subreactor中新建一个connection
    const auto connection = make_shared<Connection>(subReactors[random], socket, router, ud);
    const function<void(int)> cb = [this](const int clientFd) {
        this->DeleteConnection(clientFd);
    };
    connection->setDeleteConnectionCallBack(cb);
    connections[socket->GetFd()] = connection;
}

void ChatServer::DeleteConnection(const int clientFd) {
    printf("deleteConnection!\n");
    if (clientFd != -1) {
        if (connections.count(clientFd) > 0) {
            connections.erase(clientFd);
        }
    }
}
