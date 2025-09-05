#include "http/http_request_parser.h"
#include "http/http_request.h"
#include "http/llhttp.h"
#include "iostream"

HttpRequestParser::HttpRequestParser() : impl_(std::make_unique<Impl>()) {
    llhttp_settings_init(&impl_->settings);

    // 设置回调函数
    impl_->settings.on_message_begin = &HttpRequestParser::onMessageBegin;
    impl_->settings.on_url = &HttpRequestParser::onUrl;
    impl_->settings.on_header_field = &HttpRequestParser::onHeaderField;
    impl_->settings.on_header_value = &HttpRequestParser::onHeaderValue;
    impl_->settings.on_body = &HttpRequestParser::onBody;
    impl_->settings.on_message_complete = &HttpRequestParser::onMessageComplete;
}

HttpRequestParser::~HttpRequestParser() {
    printf("~HttpRequestParser\n");
};

void HttpRequestParser::parse(int sockfd, ParserCallback on_parsed, ErrorCallback on_error) {
    impl_->sockfd = sockfd;
    impl_->on_parsed = std::move(on_parsed);
    impl_->on_error = std::move(on_error);

    llhttp_init(&impl_->parser, HTTP_REQUEST, &impl_->settings);
    impl_->parser.data = this;

    char buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(sockfd, buffer, sizeof(buffer)))) {
        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 重置状态以便处理下一个请求（支持长连接）
                return; // 非阻塞模式下重试
            }
            impl_->on_error("Read error: " + std::string(strerror(errno)));
            return;
        }

        std::cerr << "[DEBUG] Received raw request fd (" << sockfd << ") : " << std::string(buffer, bytes_read) <<
                std::endl;

        llhttp_errno_t err = llhttp_execute(&impl_->parser, buffer, bytes_read);
        if (err != HPE_OK) {
            if (impl_->current_request) {
                std::cerr << "[ERROR] Current URL: " << impl_->current_request->url << std::endl;
                std::cerr << "[ERROR] Current Header Field: " << impl_->current_header_field << std::endl;
            }
            std::cerr << "[ERROR] Parse error: " << llhttp_errno_name(err) << " (fd: " << sockfd << ")" << std::endl;
            impl_->on_error("Parse error: " + std::string(llhttp_errno_name(err)));
            return;
        }
    }

    if (bytes_read == 0) {
        impl_->on_error("Client disconnected");
    }
}

void HttpRequestParser::reset() {
    llhttp_init(&impl_->parser, HTTP_REQUEST, &impl_->settings);
    impl_->current_request->init();
    impl_->current_header_field.clear();
}

int HttpRequestParser::onMessageBegin(llhttp_t *parser) {
    auto *self = static_cast<HttpRequestParser *>(parser->data);
    self->impl_->current_request = std::make_shared<HttpRequest>();
    return 0;
}

int HttpRequestParser::onUrl(llhttp_t *parser, const char *at, size_t length) {
    auto *self = static_cast<HttpRequestParser *>(parser->data);
    if (!self->impl_->current_request) return HPE_USER;

    self->impl_->current_request->url.assign(at, length);
    self->impl_->current_request->method = llhttp_method_name((llhttp_method_t) parser->method);

    std::cerr << "[DEBUG] Parsed method: " << self->impl_->current_request->method << std::endl;
    std::cerr << "[DEBUG] Initial method value: " << parser->method << std::endl;

    return 0;
}

int HttpRequestParser::onHeaderField(llhttp_t *parser, const char *at, size_t length) {
    auto *self = static_cast<HttpRequestParser *>(parser->data);
    if (!self->impl_->current_request) return HPE_USER;

    self->impl_->current_header_field.assign(at, length);
    return 0;
}

int HttpRequestParser::onHeaderValue(llhttp_t *parser, const char *at, size_t length) {
    auto *self = static_cast<HttpRequestParser *>(parser->data);
    if (!self->impl_->current_request) return HPE_USER;

    self->impl_->current_request->headers[self->impl_->current_header_field] = std::string(at, length);
    return 0;
}

int HttpRequestParser::onBody(llhttp_t *parser, const char *at, size_t length) {
    auto *self = static_cast<HttpRequestParser *>(parser->data);
    if (!self->impl_->current_request) return HPE_USER;

    self->impl_->current_request->body.append(at, length);
    return 0;
}

int HttpRequestParser::onMessageComplete(llhttp_t *parser) {
    auto *self = static_cast<HttpRequestParser *>(parser->data);
    if (self->impl_->on_parsed && self->impl_->current_request) {
        self->impl_->on_parsed(self->impl_->current_request);
    }
    return 0;
}
