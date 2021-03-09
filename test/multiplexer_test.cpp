#include "util/singleton.h"
#include "multiplexer.h"
#include "context.h"
#include "coroutine.h"
#include "worker.h"

#include<sys/socket.h>
#include<arpa/inet.h>

#include <iostream>

using namespace elaine;

int CreateServSock(int port) {
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(serv_sock, 3);

    return serv_sock;
}


int main(int argc, char* argv[]) {

    // auto co = std::make_shared<Coroutine>([]() {
    //     std::cout << 1 << std::endl;
    // });

    // int serv_fd = CreateServSock(8888);

    // struct sockaddr addr;
    // socklen_t socklen = sizeof(addr);
    // auto ctx = std::make_shared<AcceptContext>(co, serv_fd, &addr, &socklen);
    // g_multiplexer->RegisterContext(ctx);

    // auto result_ctx = g_multiplexer->Poll();
    // std::cout << result_ctx->GetResult() << std::endl;
    // result_ctx->GetCorouine()->Resume();

    return 0;
}