//
// Created by hubin on 25-7-20.
//

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
#include <memory>
#include "net/epoll.h"
#include "utils/timer.h"

class Epoll;
class HeapTimer;
class Channel;

class EventLoop {
public:
    std::unique_ptr<Epoll> epoll;
    bool stop;

    std::unique_ptr<HeapTimer> timer;
    int loopID;

    explicit EventLoop(int loopID);

    ~EventLoop() = default;

    void Loop() const;

    void UpdateChannel(Channel *) const;
};

#endif //EVENT_LOOP_H
