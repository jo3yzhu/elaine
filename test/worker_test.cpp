#include "worker.h"
#include "context.h"
#include "socket.h"
#include "address.h"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <string>


using namespace elaine;

auto multiplexer = Singleton<Multiplexer>::GetInstance();
int file_counter = 0;
char file_content[] = { "this is a simple simple simple simple simple simple simple test" };

void echo_func(int fd) {
    auto self = Coroutine::GetCurrent();

    char buf[16];
    size_t offset = 0;
    size_t need_read = 16;

    while (need_read) {
        ReadContext ctx(self, fd, &buf[offset], need_read);
        ctx.RegisterTo(multiplexer);

        self->Yield();

        auto recv_bytes = ctx.GetResult();
        offset += recv_bytes;
        need_read -= recv_bytes;
        std::cout << "recv client with fd " << fd << " " << recv_bytes << " bytes" << std::endl;
    }
    std::cout << "recv message client with fd: " << std::string(buf) << std::endl;

    offset = 0;
    char file_name[16];
    size_t need_write = sizeof(file_content);
    sprintf(file_name, "%d.out", file_counter++);

    int file_fd = ::open(file_name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

    while (need_write > 0) {
        WriteContext ctx(self, file_fd, &file_content[offset], need_write);
        ctx.RegisterTo(multiplexer);
        self->Yield();

        auto written_bytes = ctx.GetResult();
        offset += written_bytes;
        need_write -= written_bytes;
    }
    std::cout << "write " << file_name << " finish" << std::endl;

    offset = 0;
    need_write = 16;
    while (need_write > 0) {
        WriteContext ctx(self, fd, &buf[offset], need_write);
        ctx.RegisterTo(multiplexer);

        self->Yield();

        auto written_bytes = ctx.GetResult();
        offset += written_bytes;
        need_write -= written_bytes;
        std::cout << "send client with fd " << fd << " " << written_bytes << " bytes" << std::endl;
    }
    std::cout << "send message client with fd: " << std::string(buf) << std::endl;

    ::close(fd);
}

void accept_func(Worker::Ptr worker, Socket::Ptr socket) {
    auto self = Coroutine::GetCurrent();
    while (true) {
        struct sockaddr peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        AcceptContext ctx(self, socket->GetFd(), &peer_addr, &peer_addr_len);
        ctx.RegisterTo(multiplexer);
        self->Yield();

        int peer_fd = ctx.GetResult();
        std::cout << "peer fd:" << peer_fd << std::endl;

        auto echo_co = std::make_shared<Coroutine>(std::bind(echo_func, peer_fd));
        worker->AddTask(echo_co);
    }
}

int main(int argc, char* argv[]) {
    auto worker = std::make_shared<Worker>();
    worker->Start();

    IPAddress listen_address(1234);
    auto socket = Socket::CreateTcp();
    socket->SetReuseAddr(true);
    socket->SetReusePort(true);

    socket->Bind(listen_address);
    socket->Listen();

    auto accept_co = std::make_shared<Coroutine>(std::bind(accept_func, worker, socket));
    worker->AddTask(accept_co);

    worker->Join();

    return 0;
}