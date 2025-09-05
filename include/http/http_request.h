#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>
#include <sstream>
using namespace std;

class HttpRequest {
public:
    string method;
    string url;
    string path;
    map<string, string> queryParams;
    map<string, string> headers;
    string body;

    HttpRequest() {
        init();
    }

    ~HttpRequest() {
        printf("~HttpRequest : %s\n", url.c_str());
    }

    void init() {
        method = "";
        url = "";
        path = "";
        queryParams.clear();
        headers.clear();
        body = "";
    }

    void parseQueryParams() {
        size_t query_pos = url.find('?');
        if (query_pos != string::npos) {
            path = url.substr(0, query_pos);
            string query_str = url.substr(query_pos + 1);
            
            istringstream iss(query_str);
            string pair;
            while (getline(iss, pair, '&')) {
                size_t eq_pos = pair.find('=');
                if (eq_pos != string::npos) {
                    string key = pair.substr(0, eq_pos);
                    string value = pair.substr(eq_pos + 1);
                    queryParams[key] = value;
                }
            }
        } else {
            path = url;
        }
    }
};

#endif