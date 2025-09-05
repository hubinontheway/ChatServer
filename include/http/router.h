#ifndef ROUTER_H
#define ROUTER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using namespace std;

class HttpRequest;
class HttpResponse;
class UserRepository;

struct Context {
    shared_ptr<HttpRequest> req;
    shared_ptr<HttpResponse> res;
    shared_ptr<UserRepository> userRepo;
};

class Router {
public:
    using HttpHandler = std::function<void(Context)>;

    // 构造函数
    Router();

    ~Router();

    // 添加路由 (支持HTTP方法)
    void AddRoute(const std::string &method, const std::string &path, const HttpHandler &handler);

    // 兼容旧版的添加路由方式
    void AddRoute(const std::string &path, std::function<void(Context)> handler);

    // 处理HTTP请求
    void HandleRequest(const Context &);

private:
    struct RouteNode {
        std::unordered_map<std::string, std::unique_ptr<RouteNode> > children;
        std::unordered_map<std::string, HttpHandler> methodHandlers;
        std::string paramName;
        bool isParam = false;
    };

    std::unique_ptr<RouteNode> root;

    // 路径分割辅助函数
    std::vector<std::string> SplitPath(const std::string &path);

    // 检查是否是参数段
    bool IsParamSegment(const std::string &segment, std::string &paramName);

    // 查找路由节点
    RouteNode *FindRouteNode(const std::vector<std::string> &pathSegments,
                             std::unordered_map<std::string, std::string> &params);
};

#endif // ROUTER_H
