//
// Created by hubin on 25-7-19.
//

#ifndef SOCKET_H
#define SOCKET_H
#include <memory>

class InetAddress;

class Socket {
    int fd;

public:
    Socket();

    explicit Socket(int fd);

    ~Socket();

    void Bind(const std::shared_ptr<InetAddress> &addr) const;

    void Listen() const;

    void SetNonBlocking() const;

    void SetReuseAddr() const;

    void SetReusePort() const;

    int Accept(const std::shared_ptr<InetAddress> &addr) const;

    int GetFd() const;
};


#endif //SOCKET_H
