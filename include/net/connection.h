#ifndef CONNECTION_H
#define CONNECTION_H

// #define TIME_OUT_MS 1000

#include <functional>
#include <memory>
using namespace std;

class EventLoop;
class Socket;
class Channel;
class Router;
class HttpRequestParser;
class UserRepository;

class Connection {
    shared_ptr<EventLoop> loop;
    shared_ptr<Socket> socket;
    shared_ptr<Channel> channel;
    shared_ptr<Router> router;
    shared_ptr<UserRepository> ud;
    shared_ptr<HttpRequestParser> parser;
    function<void(int)> deleteConnectionCallBack;

public:
    Connection(shared_ptr<EventLoop> loop, shared_ptr<Socket> socket, shared_ptr<Router> router,
               shared_ptr<UserRepository> ud);

    ~Connection();

    void handleRead();

    void setDeleteConnectionCallBack(function<void(int)> deleteConnectionCallBack);
};

#endif
