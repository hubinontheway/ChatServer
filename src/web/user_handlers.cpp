#include "web/user_handlers.h"
#include "http/router.h"
#include "http/http_response.h"
#include "http/http_request.h"
#include "repository/user_repository.h"

UserHandlers::UserHandlers(shared_ptr<Router> router) {
    RegisterRoutes(router);
}

void UserHandlers::RegisterRoutes(shared_ptr<Router> router) {
    router->AddRoute("GET", "/", [](Context context) { UserHandlers::Default(context); });
    router->AddRoute("POST", "/login", [](Context context) { UserHandlers::Login(context); });
    router->AddRoute("POST", "/register", [](Context context) { UserHandlers::Register(context); });
}

void UserHandlers::Default(Context context) {
    context.res->sendFilePath = "./profile/login.html";
}

void UserHandlers::Login(Context context) {
    json jsonRequest;
    json jsonResponse;
    if (context.req->body.empty()) {
        jsonResponse["success"] = false;
        jsonResponse["message"] = "invalid JSON format!";
    } else {
        try {
            jsonRequest = json::parse(context.req->body);

            if (!jsonRequest.contains("email") || !jsonRequest.contains("password") || jsonRequest["email"].is_null() ||
                jsonRequest["password"].is_null()) {
                jsonResponse["success"] = false;
                jsonResponse["message"] = "missing email or password!";
            } else {
                auto user = context.userRepo->getUserByEmail(jsonRequest["email"]);
                if (user == nullptr) {
                    jsonResponse["success"] = false;
                    jsonResponse["message"] = "email not exists!";
                } else {
                    if (jsonRequest["password"] == user->password) {
                        jsonResponse["success"] = true;
                        jsonResponse["message"] = "login success!";
                    } else {
                        jsonResponse["success"] = false;
                        jsonResponse["message"] = "login fail!";
                    }
                }
            }
        } catch (const json::parse_error &e) {
            jsonResponse["success"] = false;
            jsonResponse["message"] = "invalid JSON format!";
        }

        context.res->headers["Content-Type"] = "application/json";
        context.res->body = jsonResponse.dump();
    }
}

void UserHandlers::Register(Context context) {
    json jsonRequest;
    json jsonResponse;

    if (context.req->body.empty()) {
        jsonResponse["success"] = false;
        jsonResponse["message"] = "invalid JSON format!";
    } else {
        try {
            jsonRequest = json::parse(context.req->body);

            if (jsonRequest.contains("email") && !jsonRequest["email"].empty() &&
                jsonRequest.contains("username") && !jsonRequest["username"].empty() &&
                jsonRequest.contains("password") && !jsonRequest["password"].empty()
            ) {
                context.userRepo->createUser(jsonRequest["email"], jsonRequest["username"], jsonRequest["password"]);
                jsonResponse["success"] = true;
                jsonResponse["message"] = "create account success!";
            } else {
                jsonResponse["success"] = false;
                jsonResponse["message"] = "internal error!";
            }
        } catch (const json::parse_error &e) {
            context.res->body = "invalid JSON format!";
        }
    }
    context.res->headers["Content-Type"] = "application/json";
    context.res->body = jsonResponse.dump();
}
