#include "multiplexer.h"

namespace elaine
{

Multiplexer::Multiplexer(unsigned depth)
    : queue_depth_(depth) {
    ::io_uring_queue_init(queue_depth_, &ring_, 0);

    // register eventfd in event loop to support wakeup
    event_fd_ = eventfd(0, 0);
    event_ctx_ = std::make_shared<EventContext>(this, nullptr, event_fd_);
    event_ctx_->RegisterSelf();
}

Multiplexer::~Multiplexer() {
    ::io_uring_queue_exit(&ring_);
    ::close(event_fd_);
}

struct io_uring* Multiplexer::GetRing() {
    return &ring_;
}

Context::Ptr Multiplexer::Poll() {
    struct io_uring_cqe* cqe;

    is_polling_ = true;
    ::io_uring_wait_cqe(&ring_, &cqe);
    ::io_uring_cqe_seen(&ring_, cqe);
    is_polling_ = false;

    auto ctx = reinterpret_cast<Context*>(cqe->user_data);
    ctx->res_ = cqe->res;
    ctx->CheckOut();

    return ctx->shared_from_this();
}

void Multiplexer::Wakeup() {
    if (!is_polling_) {
        return;
    }

    uint64_t buffer = 1;
    ::write(event_fd_, &buffer, sizeof(buffer));
}

}