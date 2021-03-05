#pragma once

#include <netinet/in.h>
#include <string>

namespace elaine
{

class IPAddress {
public:
    IPAddress() = default;
    IPAddress& operator=(const IPAddress&) = default;
    IPAddress(std::string ip, in_port_t port);
    IPAddress(in_port_t port);
    IPAddress(const struct sockaddr_in& addr);

    std::string ToString() const;
    struct sockaddr* GetSockAddr();
    struct sockaddr_in addr_;
};

}
