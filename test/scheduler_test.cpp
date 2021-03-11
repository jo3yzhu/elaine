#include "scheduler.h"
#include "socket.h"

#include <iostream>

using namespace elaine;

void echo_func(int fd) {
    // keep the only one smart pointer with count-ref on stack using shared_from_this
    // so once the function return, corresponding coroutine would be destructed

    auto self = Coroutine::GetCurrent();
    auto multiplexer = Multiplexer::GetCurrent();

    while (true) {
        char buf[16];
        size_t offset = 0;
        size_t need_read = 16;
        size_t need_write = 16;
        while (need_read) {
            ReadContext ctx(self, fd, &buf[offset], need_read);
            ctx.RegisterTo(multiplexer);
            self->Yield();

            auto recv_bytes = ctx.GetResult();

            if (recv_bytes <= 0) {
                ::close(fd);
                return;
            }
            offset += recv_bytes;
            need_read -= recv_bytes;
        }

        offset = 0;
        while (need_write) {
            WriteContext ctx(self, fd, &buf[offset], need_write);
            ctx.RegisterTo(multiplexer);
            self->Yield();

            auto written_bytes = ctx.GetResult();

            offset += written_bytes;
            need_write -= written_bytes;
        }
    }
}

void accept_func(Scheduler::Ptr echoer, Socket::Ptr socket) {
    auto self = Coroutine::GetCurrent();
    auto multiplexer = Multiplexer::GetCurrent();

    while (true) {
        struct sockaddr peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        AcceptContext ctx(self, socket->GetFd(), &peer_addr, &peer_addr_len);
        ctx.RegisterTo(multiplexer);
        self->Yield();

        int peer_fd = ctx.GetResult();


        auto echo_co = std::make_shared<Coroutine>(std::bind(echo_func, peer_fd), "echoco");

        echoer->AddTask(echo_co);
    }
}

void BlockSigno(int signo) {
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, signo);
    pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
}

int main(int argc, char* argv[]) {
    BlockSigno(SIGPIPE);

    IPAddress listen_address(1234);
    auto socket = Socket::CreateTcp();
    socket->SetReuseAddr(true);
    socket->SetReusePort(true);
    socket->Bind(listen_address);
    socket->Listen();

    auto acceptor = std::make_shared<Scheduler>(1, "acceptor");
    auto echoer = std::make_shared<Scheduler>(3, "echoer");

    auto accept_co = std::make_shared<Coroutine>(std::bind(accept_func, echoer, socket), "acceptco");

    echoer->Start();
    acceptor->Start();
    acceptor->AddTask(accept_co);
    acceptor->Wait();

    return 0;
}