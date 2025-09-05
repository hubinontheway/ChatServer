//
// Created by hubin on 25-7-19.
//
#include <iostream>
#include <unistd.h>
#include <net/socket.h>
#include <sys/socket.h>

#include "net/inet_address.h"
#include "utils/utils.h"

Socket::Socket() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "Socket creation failed." << std::endl;
    }
}

Socket::Socket(const int fd): fd(fd) {
    if (fd < 0) {
        std::cout << "Socket creation failed." << std::endl;
    }
}

Socket::~Socket() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

void Socket::Bind(const std::shared_ptr<InetAddress> &addr) const {
    if (bind(fd, reinterpret_cast<sockaddr *>(&addr->addr), addr->addrLen) < 0) {
        perror("bind");
        exit(1);
    }
}

void Socket::Listen() const {
    if (listen(fd, SOMAXCONN) < 0) {
        std::cout << "Socket listen failed." << std::endl;
    }
}

void Socket::SetNonBlocking() const {
    Utils::SetNonBlocking(fd);
}

void Socket::SetReuseAddr() const {
    constexpr int reuseAddr = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));
}

void Socket::SetReusePort() const {
    constexpr int reusePort = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(int));
}

int Socket::Accept(const std::shared_ptr<InetAddress> &addr) const {
    const int clientSocket = accept4(fd, reinterpret_cast<sockaddr *>(&addr->addr), &addr->addrLen,
                                     SOCK_CLOEXEC | SOCK_NONBLOCK);
    if (clientSocket == -1) {
        perror("socket accept error");
    } else {
        printf("[DEBUG] Accepted new client fd: %d\n", clientSocket);
    }
    return clientSocket;
}

int Socket::GetFd() const {
    return fd;
}
