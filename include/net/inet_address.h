//
// Created by hubin on 25-7-20.
//

#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <cstring>
#include <strings.h>
#include <arpa/inet.h>

class InetAddress {
public:
    sockaddr_in addr{};
    socklen_t addrLen;

    InetAddress(): addrLen(sizeof(addr)) {
        bzero(&addr, sizeof(addr));
    }

    InetAddress(const char *ip, const uint16_t port): addrLen(sizeof(addr)) {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &addr.sin_addr);
    }
};

#endif //INET_ADDRESS_H
