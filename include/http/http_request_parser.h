#ifndef HTTP_REQUEST_PARSER_H
#define HTTP_REQUEST_PARSER_H

#include <functional>
#include <memory>
#include <string>
#include <map>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include "llhttp.h"

using namespace std;

class HttpRequest;
class HttpRequestParser {
public:
    using ParserCallback = function<void(shared_ptr<HttpRequest>)>;
    using ErrorCallback = function<void(const string&)>;

    HttpRequestParser();
    ~HttpRequestParser();

    // 开始解析请求
    void parse(int sockfd, ParserCallback on_parsed, ErrorCallback on_error);

    // 重置解析器状态
    void reset();

private:
    // llhttp 解析器回调
    static int onMessageBegin(llhttp_t* parser);
    static int onUrl(llhttp_t* parser, const char* at, size_t length);
    static int onHeaderField(llhttp_t* parser, const char* at, size_t length);
    static int onHeaderValue(llhttp_t* parser, const char* at, size_t length);
    static int onBody(llhttp_t* parser, const char* at, size_t length);
    static int onMessageComplete(llhttp_t* parser);

public:
    // 内部状态
    struct Impl {
        llhttp_t parser;
        llhttp_settings_t settings;
        std::shared_ptr<HttpRequest> current_request;
        std::string current_header_field;
        ParserCallback on_parsed;
        ErrorCallback on_error;
        int sockfd;
    };
    unique_ptr<Impl> impl_;
};

#endif
