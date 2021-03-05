#include "multiplexer.h"

#include <iostream>

namespace elaine
{

Multiplexer::Multiplexer()
    : queue_depth_(DefaultRingDepth) {
    ::io_uring_queue_init(queue_depth_, &ring_, 0);
}

Multiplexer::~Multiplexer() {
    ::io_uring_queue_exit(&ring_);
}

struct io_uring* Multiplexer::GetRing() {
    return &ring_;
}

Context* Multiplexer::Poll() {
    struct io_uring_cqe* cqe;

    is_polling_ = true;
    auto error = ::io_uring_wait_cqe(&ring_, &cqe);
    ::io_uring_cqe_seen(&ring_, cqe);
    is_polling_ = false;
    if (cqe->res < 0) {
        std::cout << errno << std::endl;
    }

    auto ctx = reinterpret_cast<Context*>(cqe->user_data);
    ctx->res_ = cqe->res;
    ctx->status_ = error == 0 ? Context::Status::kSuccess : Context::Status::kFail;

    return ctx;
}

bool Multiplexer::IsPolling() {
    return is_polling_;
}

}