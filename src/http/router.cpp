#include "http/router.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "repository/user_repository.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

Router::Router() : root(make_unique<RouteNode>()) {
}

Router::~Router() = default;

vector<string> Router::SplitPath(const string &path) {
    vector<string> segments;
    stringstream ss(path);
    string segment;

    while (getline(ss, segment, '/')) {
        if (!segment.empty()) {
            segments.push_back(segment);
        }
    }
    return segments;
}

bool Router::IsParamSegment(const string &segment, string &paramName) {
    if (segment.size() > 1 && segment[0] == ':') {
        paramName = segment.substr(1);
        return true;
    }
    return false;
}

void Router::AddRoute(const string &method, const string &path, const HttpHandler &handler) {
    cout << "Registering route: " << method << " " << path << endl;
    const auto segments = SplitPath(path);
    RouteNode *current = root.get();

    for (const string &segment: segments) {
        string paramName;
        if (IsParamSegment(segment, paramName)) {
            if (!current->children[""]) {
                current->children[""] = make_unique<RouteNode>();
                current->children[""]->isParam = true;
                current->children[""]->paramName = paramName;
            }
            current = current->children[""].get();
        } else {
            if (current->children.find(segment) == current->children.end()) {
                current->children[segment] = make_unique<RouteNode>();
            }
            current = current->children[segment].get();
        }
    }

    current->methodHandlers[method] = handler;
}

void Router::AddRoute(const string &path, function<void(Context)> handler) {
    // 适配旧版HandlerFunction到新版HttpHandler
    AddRoute("GET", path, handler);
}

Router::RouteNode *Router::FindRouteNode(const vector<string> &pathSegments, unordered_map<string, string> &params) {
    printf("findRouteNode!\n");
    RouteNode *current = root.get();

    for (const string &segment: pathSegments) {
        // 1. 尝试匹配静态路径
        if (current->children.find(segment) != current->children.end()) {
            current = current->children[segment].get();
        }
        // 2. 尝试匹配参数路径
        else if (current->children.find("") != current->children.end()) {
            params[current->children[""]->paramName] = segment;
            current = current->children[""].get();
        }
        // 3. 没有匹配项
        else {
            return nullptr;
        }
    }

    return current;
}

void Router::HandleRequest(const Context &context) {
    const auto segments = SplitPath(context.req->path);
    unordered_map<string, string> routeParams;

    RouteNode *node = FindRouteNode(segments, routeParams);
    if (node == nullptr) {
        // 默认处理器逻辑
        const string filePath = "profile" + context.req->path; // 假设静态文件在 profile 目录下
        auto ifFile = [](const string &filePath) {
            struct stat st{};
            if (stat(filePath.c_str(), &st) != 0) {
                return false;
            }
            return S_ISREG(st.st_mode);
        };
        if (!ifFile(filePath)) {
            context.res->status_code = 404;
            context.res->body = "404 Not Found: The requested resource could not be found.";
            return;
        }
        context.res->sendFilePath = filePath;
        return;
    }

    // 将路径参数存入请求对象
    for (const auto &param: routeParams) {
        context.req->queryParams[param.first] = param.second;
    }

    // 查找匹配的HTTP方法处理器
    const auto it = node->methodHandlers.find(context.req->method);
    if (it != node->methodHandlers.end()) {
        cout << "Found handler for path: " << context.req->path
                << " method: " << context.req->method << endl;
        it->second(context);
    } else {
        cout << "No handler found for path: " << context.req->path
                << " method: " << context.req->method << endl;
        context.res->status_code = 405; // Method Not Allowed
        context.res->body = "405 Method Not Allowed";
    }
}
