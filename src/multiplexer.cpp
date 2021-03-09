#include "multiplexer.h"
#include "thread.h"
#include "worker.h"

#include <iostream>

namespace elaine
{

thread_local Multiplexer* t_multiplexer = nullptr;

Multiplexer::Multiplexer()
    : queue_depth_(DefaultRingDepth) {
    ::io_uring_queue_init(queue_depth_, &ring_, 0);
}

Multiplexer::~Multiplexer() {
    ::io_uring_queue_exit(&ring_);
}

void Multiplexer::SetCurrent(Multiplexer *multiplexer) {
    t_multiplexer = multiplexer;
}

Multiplexer* Multiplexer::GetCurrent() {
    return t_multiplexer;
}

struct io_uring* Multiplexer::GetRing() {
    return &ring_;
}

Context* Multiplexer::Poll() {
    struct io_uring_cqe* cqe = nullptr;

    is_polling_ = true;
    auto error = ::io_uring_wait_cqe(&ring_, &cqe);
    ::io_uring_cqe_seen(&ring_, cqe);
    is_polling_ = false;

    auto ctx = reinterpret_cast<Context*>(cqe->user_data);
    if(ctx->co_ == Worker::GetCurrent()->event_ctx_.co_) {
        assert(ctx->co_->GetStatus() == Coroutine::Status::kRunning);
        return nullptr;
    }


    ctx->res_ = cqe->res;
    ctx->status_ = error == 0 ? Context::Status::kSuccess : Context::Status::kFail;

    return ctx;
}

bool Multiplexer::IsPolling() {
    return is_polling_;
}

}