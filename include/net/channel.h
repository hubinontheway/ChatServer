//
// Created by hubin on 25-7-20.
//

#ifndef CHANNEL_H
#define CHANNEL_H
#include <functional>
#include <memory>

class EventLoop;

class Channel {
public:
    std::shared_ptr<EventLoop> loop;
    std::function<void()> callback;
    uint32_t event;
    uint32_t readyEvent;
    bool inEpoll;
    int fd;

    Channel(const std::shared_ptr<EventLoop> &loop, int fd);

    ~Channel() = default;

    void HandleEvent() const;

    void EnableReading();

    void DisableReading();

    int GetFd() const;

    uint32_t GetEvent() const;

    uint32_t GetReadyEvent() const;

    bool GetInEpoll() const;

    void SetInEpoll();

    void SetEvent(uint32_t event);

    // void SetReadyEvent(uint32_t readyEvent);

    // 对应的事件触发回调函数
    void SetCallBack(const std::function<void()> &callback);
};

#endif //CHANNEL_H
