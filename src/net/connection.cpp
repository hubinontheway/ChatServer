#include "net/connection.h"
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/channel.h"
#include "http/llhttp.h"
#include "http/http_request_parser.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/router.h"
#include "utils/timer.h"
#include "repository/user_repository.h"

Connection::Connection(shared_ptr<EventLoop> loop,
                       shared_ptr<Socket> socket,
                       shared_ptr<Router> router,
                       shared_ptr<UserRepository> ud)
    : loop(loop), socket(socket), router(router), ud(ud), deleteConnectionCallBack([](int) {
    }) {
    printf("new Connection fd : %d to loop (%d)\n", socket->GetFd(), loop->stop);
    channel = make_shared<Channel>(loop, socket->GetFd());
    function<void()> cb = [this]() {
        this->handleRead();
    };
    channel->SetCallBack(cb);
    channel->EnableReading();

    parser = make_shared<HttpRequestParser>();

    loop->timer->Add(socket->GetFd(), TIME_OUT_MS, [this]() {
        printf("timer triggered, connection : %d closed! \n", this->socket->GetFd());
        this->deleteConnectionCallBack(this->socket->GetFd());
    });
}

Connection::~Connection() {
    printf("~Connection!\n");
}

void Connection::handleRead() {
    bool keepAlive = false;
    bool parseError = false;

    loop->timer->Adjust(this->socket->GetFd(), TIME_OUT_MS);
    parser->parse(socket->GetFd(),
                  [this, &keepAlive](shared_ptr<HttpRequest> request) {
                      // 解析完成后的回调函数
                      // 路由到处理函数并写回响应
                      if (request->headers["Connection"] == "keep-alive") {
                          keepAlive = true;
                      }
                      request->parseQueryParams();
                      shared_ptr<HttpResponse> response = make_shared<HttpResponse>(socket->GetFd());
                      router->HandleRequest(Context{request, response, ud});
                      printf("send response: fd (%d)!\n", socket->GetFd());
                      if (!response->body.empty()) {
                          response->sendResponseWithText();
                      } else {
                          response->sendResponseWithMappedFile();
                      }
                  },
                  [this, &parseError](const string &error) {
                      // 错误处理
                      cerr << "Parse error: " << error << " (fd: " << socket->GetFd() << ")" << endl;
                      parseError = true;
                  });

    if (parseError || !keepAlive) {
        deleteConnectionCallBack(socket->GetFd());
    } else {
        parser->reset();
    }
}

void Connection::setDeleteConnectionCallBack(function<void(int)> cb) {
    deleteConnectionCallBack = cb;
    printf("[DEBUG] deleteConnectionCallBack set for fd: %d\n", socket->GetFd());
}
