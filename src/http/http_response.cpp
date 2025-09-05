#include "http/http_response.h"
#include "utils/utils.h"
#include <memory>
#include <unistd.h>

using namespace std;

HttpResponse::HttpResponse(int fd)
    : clientFd(fd), body(""), sendFilePath("") {
}

string HttpResponse::buildHeader() const {
    string header = "HTTP/1.1 " + to_string(status_code) + " ";

    switch (status_code) {
        case 200: header += "OK";
            break;
        case 301: header += "Moved Permanently";
            break;
        case 404: header += "Not Found";
            break;
        case 500: header += "Internal Server Error";
            break;
        default: header += "Unknown";
            break;
    }
    header += "\r\n";

    // 基于响应类型设置默认头部（不覆盖已有）
    switch (responseType) {
        case ResponseType::HTML:
            if (!headers.count("Content-Type"))
                header += "Content-Type: text/html; charset=UTF-8\r\n";
            break;
        case ResponseType::JSON:
            if (!headers.count("Content-Type"))
                header += "Content-Type: application/json\r\n";
            break;
        case ResponseType::TEXT:
            if (!headers.count("Content-Type"))
                header += "Content-Type: text/plain\r\n";
            break;
        case ResponseType::REDIRECT:
            if (!headers.count("Location"))
                header += "Location: /\r\n";
            break;
        case ResponseType::IMAGE_JPEG:
            if (!headers.count("Content-Type"))
                header += "Content-Type: image/jpeg\r\n";
            break;
        case ResponseType::IMAGE_PNG:
            if (!headers.count("Content-Type"))
                header += "Content-Type: image/png\r\n";
            break;
        case ResponseType::CUSTOM:
            break;
    }

    // 添加已有 headers
    for (const auto &Header: headers) {
        header += Header.first + ": " + Header.second + "\r\n";
    }

    return header;
}

void HttpResponse::sendResponseWithText() {
    string header = buildHeader();
    if (!headers.count("Content-Length")) {
        header += "Content-Length: " + to_string(body.size()) + "\r\n";
    }
    header += "\r\n";
    header += body;
    write(clientFd, header.c_str(), header.size());
}

void *HttpResponse::mapFileToMemory(int fd, size_t size) {
    void *addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        cout << "mmap error" << endl;
    }
    return addr;
}

int HttpResponse::openFile(const string &path, struct stat &fileStat) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        cout << "open file error" << endl;
    }
    if (fstat(fd, &fileStat) == -1) {
        cout << "fstat error" << endl;
    }
    return fd;
}

void HttpResponse::sendResponseWithMappedFile() {
    struct stat fileStat;
    int fileFd = openFile(sendFilePath, fileStat);
    void *fileData = mapFileToMemory(fileFd, fileStat.st_size);

    responseType = ResponseType::HTML;
    string header = buildHeader();
    // 自动添加 Content-Length
    if (!headers.count("Content-Length")) {
        header += "Content-Length: " + to_string(fileStat.st_size) + "\r\n";
    }
    header += "\r\n";

    struct iovec iv[2];
    iv[0].iov_base = (void *) header.c_str();
    iv[0].iov_len = header.size();
    iv[1].iov_base = fileData;
    iv[1].iov_len = fileStat.st_size;

    writev(clientFd, iv, 2);
    munmap(fileData, fileStat.st_size);
}
