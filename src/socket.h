#pragma once

#include "address.h"

#include <memory>

namespace elaine
{

class Socket {
public:
    typedef std::shared_ptr<Socket> Ptr;

public:
    static Socket::Ptr CreateTcp();
    Socket(int fd) : fd_(fd) {}
    int GetFd() const { return fd_; }

    void Bind(IPAddress& local);
    void Listen();
    int Accept(IPAddress& peer_addr);
    int Connect(IPAddress& server_addr);
    ssize_t Read(void* buf, size_t count);
    ssize_t Write(const void* buf, size_t count);

    void SetTcpNoDelay(bool on);
    void SetReuseAddr(bool on);
    void SetReusePort(bool on);
    void SetKeepAlive(bool on);

    void ShutdownWrite();
    void Close();

private:
    int fd_;
};

}
