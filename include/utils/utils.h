//
// Created by hubin on 25-7-20.
//

#ifndef UTILS_H
#define UTILS_H
#include <cstdio>
#include <cstring>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdlib>

class Utils {
public:
    static void AddFdToEpoll(const int epollFd, const int fd, const bool isOneshot, const bool isListen) {
        epoll_event event{};
        event.data.fd = fd;

        if (!isListen) {
            event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        } else {
            event.events = EPOLLIN | EPOLLRDHUP;
        }

        // EPOLLONESHOT：表示在处理事件后，该文件描述符会被自动从 epoll 实例中移除
        if (isOneshot) {
            event.events |= EPOLLONESHOT;
        }

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0) {
            perror("epoll_ctl");
            exit(-1);
        }

        SetNonBlocking(fd);
    }

    static int SetNonBlocking(const int fd) {
        return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    }

    static void ModifyFd(const int epollFd, const int fd, const uint32_t addEvent) {
        epoll_event event{};
        event.data.fd = fd;
        event.events = addEvent | EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
        if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event) < 0) {
            perror("EPOLL_CTL_MOD");
            exit(-1);
        }
    }

    static void AddFd(const int epollFd, const int fd, void *ptr, const uint32_t addEvent) {
        epoll_event event{};
        memset(&event, 0, sizeof(event));
        event.data.fd = fd;
        event.data.ptr = ptr;
        event.events = addEvent;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0) {
            perror("EPOLL_CTL_ADD");
            exit(-1);
        }
    }

    static void RemoveFd(const int epollFd, const int fd) {
        if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
            perror("EPOLL_CTL_DEL");
            exit(-1);
        }
    }
};

#endif //UTILS_H
