//
// Created by hubin on 25-7-20.
//
#include <unistd.h>
#include "net/epoll.h"
#include "utils/utils.h"
#include <cstdio>
#include <cstring>

#define MAX_EVENT_NUMBER 10000

Epoll::Epoll() {
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        perror("epoll_create1");
    }
    epollEvents = std::make_unique<epoll_event[]>(MAX_EVENT_NUMBER);
}

Epoll::~Epoll() {
    if (epollFd != -1) {
        close(epollFd);
        epollFd = -1;
    }
}

std::vector<Channel *> Epoll::Poll(const int timeout) const {
    std::vector<Channel *> activeChannels;
    const int nFds = epoll_wait(epollFd, epollEvents.get(), MAX_EVENT_NUMBER, timeout);
    if (nFds == -1) {
        perror("epoll_wait");
    }
    for (int count = 0; count < nFds; ++count) {
        auto *channel = static_cast<Channel *>(epollEvents[count].data.ptr);
        channel->SetEvent(epollEvents[count].events);
        activeChannels.push_back(channel);
    }
    return activeChannels;
}

void Epoll::UpdateChannel(Channel *channel) const {
    const int fd = channel->GetFd();
    const uint32_t event = channel->GetEvent();
    if (!channel->GetInEpoll()) {
        Utils::AddFd(epollFd, fd, channel, event);
        channel->SetInEpoll();
    } else if (event == 0) {
        // 如果事件为0，删除该事件
        Utils::RemoveFd(epollFd, fd);
    } else {
        Utils::ModifyFd(epollFd, fd, event);
    }
}
