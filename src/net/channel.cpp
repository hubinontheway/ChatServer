//
// Created by hubin on 25-7-20.
//
#include "net/channel.h"

#include "net/event_loop.h"
#include <sys/epoll.h>

Channel::Channel(const std::shared_ptr<EventLoop> &loop, const int fd): event(0), readyEvent(0), inEpoll(false) {
    this->loop = loop;
    this->fd = fd;
}

void Channel::EnableReading() {
    event = EPOLLIN | EPOLLET;
    loop->UpdateChannel(this);
}

void Channel::DisableReading() {
    event = 0;
    loop->UpdateChannel(this);
}

int Channel::GetFd() const {
    return fd;
}

uint32_t Channel::GetEvent() const {
    return event;
}

uint32_t Channel::GetReadyEvent() const {
    return readyEvent;
}

bool Channel::GetInEpoll() const {
    return inEpoll;
}

void Channel::SetInEpoll() {
    this->inEpoll = true;
}

void Channel::SetEvent(const uint32_t event) {
    this->event = event;
}

void Channel::SetCallBack(const std::function<void()> &callback) {
    this->callback = callback;
}

void Channel::HandleEvent() const {
    callback();
}
