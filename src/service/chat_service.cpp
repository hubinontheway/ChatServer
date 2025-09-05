#include "service/chat_service.h"
#include <iostream>
#include <curl/curl.h>


ChatService::ChatService() = default;

string ChatService::getChatResponse(const string &question, const string &model) {
    CURLcode curlCode;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), &curl_easy_cleanup);

    if (curl != nullptr) {
        // 只能使用裸指针
        curl_slist *headers = nullptr;
        string apiKey = "yours_key";
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        json jsonRequest = {
            {"model", model},
            {
                "messages", {
                    {{"role", "user"}, {"content", question}}
                }
            }
        };

        string requestBody = jsonRequest.dump();

        curl_easy_setopt(curl.get(), CURLOPT_URL, "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
        curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);
        curlCode = curl_easy_perform(curl.get());

        json jsonResponse;
        if (curlCode != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(curlCode) << std::endl;
        } else {
            try {
                jsonResponse = json::parse(response);
                if (jsonResponse.contains("choices") && !jsonResponse["choices"].empty() &&
                    jsonResponse["choices"][0].contains("message") &&
                    jsonResponse["choices"][0]["message"].contains("content")) {
                    response = jsonResponse["choices"][0]["message"]["content"];
                } else {
                    std::cerr << "Unexpected JSON structure: " << jsonResponse.dump() << std::endl;
                }
            } catch (json::parse_error &e) {
                std::cerr << "Failed to parse the JSON response: " << e.what() << std::endl;
            }
        }
        // curl_easy_cleanup(curl.get());
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
    return response;
}
