#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <memory>
using namespace std;

enum class ResponseType {
    HTML,
    JSON,
    TEXT,
    REDIRECT,
    IMAGE_JPEG,
    IMAGE_PNG,
    CUSTOM
};

class HttpResponse {
public:
    int clientFd;
    int status_code = 200;
    map<string, string> headers;
    string body;
    string sendFilePath;
    ResponseType responseType = ResponseType::CUSTOM;
    
public:
    HttpResponse(int);
    string buildHeader() const;
    void* mapFileToMemory(int fd, size_t size);
    int openFile(const string& path, struct stat& fileStat);
    void sendResponseWithMappedFile();
    void sendResponseWithText();
};

#endif