#include "socket.h"


#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>

namespace elaine
{

Socket::Ptr Socket::CreateTcp() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert(fd != -1);
    return std::make_shared<Socket>(fd);
}

void Socket::Bind(IPAddress& local) {
    int error = ::bind(fd_, local.GetSockAddr(), sizeof(struct sockaddr_in));
    assert(error == 0);
}

void Socket::Listen() {
    int error = ::listen(fd_, SOMAXCONN);
    assert(error == 0);
}

int Socket::Accept(IPAddress& peer_addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr));
    int peer_fd = ::accept(fd_, peer_addr.GetSockAddr(), &addrlen);
    assert(peer_fd != -1);
    return peer_fd;
}

int Socket::Connect(IPAddress& server_addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr));
    return ::connect(fd_, server_addr.GetSockAddr(), addrlen);
}

ssize_t Socket::Read(void* buf, size_t count) {
    return ::read(fd_, buf, count);
}

ssize_t Socket::Write(const void* buf, size_t count) {
    return ::write(fd_, buf, count);
}

void Socket::SetTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    int error = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
    assert(error != -1);
}

void Socket::SetReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    int error = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    assert(error != -1);
}

void Socket::SetReusePort(bool on) {
    int optval = on ? 1 : 0;
    int error = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    assert(error != -1);
}

void Socket::SetKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    int error = ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
    assert(error != -1);
}


void Socket::ShutdownWrite() {
	int error = ::shutdown(fd_, SHUT_WR);
	assert(error != -1);
}

void Socket::Close() {
	::close(fd_);
}

}