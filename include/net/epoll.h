//
// Created by hubin on 25-7-20.
//

#ifndef EPOLL_H
#define EPOLL_H
#include <memory>
#include <vector>
#include <sys/epoll.h>
#include <net/channel.h>
class Channel;

class Epoll {
public:
    int epollFd;
    std::unique_ptr<epoll_event[]> epollEvents;

    Epoll();

    ~Epoll();

    std::vector<Channel *> Poll(int timeout) const;

    void UpdateChannel(Channel *channel) const;
};

#endif //EPOLL_H
