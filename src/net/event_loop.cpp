//
// Created by hubin on 25-7-20.
//
#include <vector>
#include "net/event_loop.h"
#include "net/epoll.h"
#include "utils/timer.h"

EventLoop::EventLoop(const int loopID) {
    this->stop = false;
    this->loopID = loopID;
    this->epoll = std::make_unique<Epoll>();
    this->timer = std::make_unique<HeapTimer>();
}

void EventLoop::Loop() const {
    while (!this->stop) {
        const int timeMS = timer->GetNextTick();
        std::vector<Channel *> activeChannels = epoll->Poll(timeMS);
        for (const auto it: activeChannels) {
            it->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *channel) const {
    epoll->UpdateChannel(channel);
}
