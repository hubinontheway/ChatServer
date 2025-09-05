#pragma once

#include <memory>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace std;

class Router;
struct Context;
class UserHandlers {
public:
    UserHandlers(shared_ptr<Router> router);

public:
    void RegisterRoutes(shared_ptr<Router> router);

    static void Default(Context context);
    static void Login(Context context);
    static void Register(Context context);
};