#include "context.h"
#include "multiplexer.h"

namespace elaine
{

Context::Context(Multiplexer* multiplexer, Coroutine::Ptr co, int fd, Event event)
    : multiplexer_(multiplexer), co_(co), fd_(fd), event_(event) {
}

Coroutine::Ptr Context::GetCorouine() {
    return co_;
}

Context::Event Context::GetEvent() {
    return event_;
}

Context::Status Context::GetStatus() {
    return status_;
}

EventContext::EventContext(Multiplexer* multiplexer, Coroutine::Ptr co, int fd)
    : Context(multiplexer, co, fd, Context::Event::kAccept) {
}

void EventContext::RegisterSelf() {
    auto ring = multiplexer_->GetRing();
    auto sqe = ::io_uring_get_sqe(ring);
    assert(sqe != nullptr);

    ::io_uring_prep_read(sqe, fd_, &buffer_, sizeof(buffer_), 0);
    ::io_uring_sqe_set_data(sqe, this);
    ::io_uring_submit(ring);

    status_ = Status::kPolling;
}

// @description: check if accept system call valid
// @note: called by multiplexer after polling return

void EventContext::CheckOut() {
    assert(event_ == Event::kEvent);
    assert(status_ == Status::kPolling);
    status_ = Status::kSuccess;
}

AcceptContext::AcceptContext(Multiplexer* multiplexer, Coroutine::Ptr co, int fd)
    : Context(multiplexer, co, fd, Context::Event::kAccept) {
}

void AcceptContext::RegisterSelf() {
    auto ring = multiplexer_->GetRing();
    auto sqe = ::io_uring_get_sqe(ring);
    assert(sqe != nullptr);

    socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr));
    ::io_uring_prep_accept(sqe, fd_, &peer_addr_, &addrlen, 0);
    ::io_uring_sqe_set_data(sqe, this);
    ::io_uring_submit(ring);

    status_ = Status::kPolling;
}

// @description: check if accept system call valid
// @note: called by multiplexer after polling return

void AcceptContext::CheckOut() {
    assert(event_ == Event::kAccept);
    assert(status_ == Status::kPolling);
    status_ = res_ < 0 ? Status::kFail : Status::kSuccess;
}

}