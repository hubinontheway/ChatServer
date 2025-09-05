//
// Created by hubin on 25-7-23.
//

#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include <functional>
#include <memory>
#include <cstdio>
#include "net/socket.h"
#include "net/inet_address.h"
#include "net/channel.h"

using namespace std;

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Connection;

class Acceptor {
    shared_ptr<EventLoop> acceptLoop;
    shared_ptr<Socket> serverSocket;
    shared_ptr<InetAddress> listenAddr;
    shared_ptr<Channel> acceptorChannel;

public:
    explicit Acceptor(const shared_ptr<EventLoop> &acceptLoop) {
        this->acceptLoop = acceptLoop;
        this->serverSocket = make_shared<Socket>();
        this->listenAddr = make_shared<InetAddress>("0.0.0.0", 5342);
        // 需要在绑定之前设置
        this->serverSocket->SetReuseAddr();
        this->serverSocket->SetReusePort();
        this->serverSocket->Bind(this->listenAddr);
        this->serverSocket->Listen();
        this->serverSocket->SetNonBlocking();
        this->acceptorChannel = make_shared<Channel>(this->acceptLoop, this->serverSocket->GetFd());
        auto acceptCallback = [this]() {
            this->AcceptConnection();
        };
        this->acceptorChannel->SetCallBack(acceptCallback);
        this->acceptorChannel->EnableReading();
    }

    ~Acceptor() = default;

    void AcceptConnection() const {
        const auto clientAddr = make_shared<InetAddress>();
        const auto clientSocket = make_shared<Socket>(this->serverSocket->Accept(clientAddr));
        printf("[DEBUG] New client fd: %d, IP: %s, Port: %d\n", clientSocket->GetFd(),
               inet_ntoa(clientAddr->addr.sin_addr), ntohs(clientAddr->addr.sin_port));

        if (!this->newConnectionCallBack) {
            fprintf(stderr, "[ERROR] New connection callback is not set.\n");
            return;
        }

        if (clientSocket->GetFd() == -1) {
            fprintf(stderr, "[ERROR] Failed to accept new connection.\n");
            return;
        }

        this->newConnectionCallBack(clientSocket);
    }

    void SetNewConnectionCallBack(const function<void(shared_ptr<Socket>)> &callback) {
        this->newConnectionCallBack = callback;
    }

    function<void(shared_ptr<Socket>)> newConnectionCallBack;
};

#endif //ACCEPTOR_H
