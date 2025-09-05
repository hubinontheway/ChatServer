#pragma once

#include <memory>

using namespace std;

class ChatService;
class Router;
struct Context;
class ChatHandlers {
public:
    ChatHandlers(shared_ptr<Router> router);

public:
    void RegisterRoutes(shared_ptr<Router>);
    static void sendMessageAndGetResponse(Context context);
};