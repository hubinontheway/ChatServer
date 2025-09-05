#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace std;

class ChatService {
public:
    ChatService();

public:
    static string getChatResponse(const string &question, const string &model);

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        ((std::string *) userp)->append((char *) contents, size * nmemb);
        return size * nmemb;
    }
};
