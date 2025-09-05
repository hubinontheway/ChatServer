#include "web/chat_handlers.h"
#include "http/router.h"
#include "service//chat_service.h"
#include "http/http_request.h"
#include "http/http_response.h"

ChatHandlers::ChatHandlers(shared_ptr<Router> router) {
    RegisterRoutes(router);
}

void ChatHandlers::RegisterRoutes(shared_ptr<Router> router) {
    router->AddRoute("POST", "/send-message", [](Context context) {
        ChatHandlers::sendMessageAndGetResponse(context);
    });
}

void ChatHandlers::sendMessageAndGetResponse(Context context) {
    json jsonRequest;
    json jsonResponse;
    if (context.req->body.empty()) {
        jsonResponse["name"] = "None";
        jsonResponse["message"] = "invalid JSON format!";
    } else {
        try {
            jsonRequest = json::parse(context.req->body);

            if (!jsonRequest.contains("username") || jsonRequest["username"].is_null() ||
                !jsonRequest.contains("message") || jsonRequest["message"].is_null() ||
                !jsonRequest.contains("model") || jsonRequest["model"].is_null()
            ) {
                jsonResponse["name"] = "None";
                jsonResponse["message"] = "missing email or password!";
            } else {
                string response = ChatService::getChatResponse(jsonRequest["message"], jsonRequest["model"]);
                if (response.empty()) {
                    jsonResponse["name"] = jsonRequest["model"];
                    jsonResponse["message"] = "request get wrong!";
                } else {
                    jsonResponse["name"] = jsonRequest["model"];
                    jsonResponse["message"] = response;
                }
            }
        } catch (const json::parse_error &e) {
            jsonResponse["name"] = "None";
            jsonResponse["message"] = "invalid JSON format!";
        }

        context.res->headers["Content-Type"] = "application/json";
        context.res->body = jsonResponse.dump();
    }
}
