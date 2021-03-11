#include "util/singleton.h"
#include "context.h"
#include "multiplexer.h"

namespace elaine
{

Context::Context(Coroutine::Ptr co, int fd, Event event)
    : co_(co), fd_(fd), event_(event) {
}

Coroutine::Ptr Context::GetCorouine() {
    return co_;
}

int Context::GetResult() {
    return res_;
}

int Context::GetFd() {
    return fd_;
}

Context::Event Context::GetEvent() {
    return event_;
}

Context::Status Context::GetStatus() {
    return status_;
}

pid_t Context::GetThreadId() {
    return thread_id_;
}

NopContext::NopContext()
    : Context(nullptr, -1, Context::Event::kNop) {
}

void NopContext::RegisterTo(Multiplexer* multiplexer) {
    auto ring = multiplexer->GetRing();
    auto sqe = ::io_uring_get_sqe(ring);
    assert(sqe != nullptr);

    ::io_uring_prep_nop(sqe);
    ::io_uring_sqe_set_data(sqe, this);
    auto submit_num = ::io_uring_submit(ring);
    assert(submit_num == 1);
}

AcceptContext::AcceptContext(Coroutine::Ptr co, int sockfd, struct sockaddr* addr, socklen_t* addrlen)
    : Context(co, sockfd, Context::Event::kAccept), addr_(addr), addrlen_(addrlen) {
}

void AcceptContext::RegisterTo(Multiplexer* multiplexer) {
    auto ring = multiplexer->GetRing();
    auto sqe = ::io_uring_get_sqe(ring);
    assert(sqe != nullptr);

    ::io_uring_prep_accept(sqe, fd_, addr_, addrlen_, 0);
    ::io_uring_sqe_set_data(sqe, this);
    auto submit_num = ::io_uring_submit(ring);
    assert(submit_num == 1);

    status_ = Status::kPolling;
}

ReadvContext::ReadvContext(Coroutine::Ptr co, int fd, struct iovec* iov, int iovcnt)
    : Context(co, fd, Context::Event::kReadv), iov_(iov), iovcnt_(iovcnt) {
}

void ReadvContext::RegisterTo(Multiplexer* multiplexer) {
    auto ring = multiplexer->GetRing();
    auto sqe = ::io_uring_get_sqe(ring);
    assert(sqe != nullptr);

    ::io_uring_prep_readv(sqe, fd_, iov_, iovcnt_, 0);
    ::io_uring_sqe_set_data(sqe, this);
    auto submit_num = ::io_uring_submit(ring);
    assert(submit_num == 1);

    status_ = Status::kPolling;
}

WritevContext::WritevContext(Coroutine::Ptr co, int fd, const struct iovec* iov, int iovcnt)
    : Context(co, fd, Context::Event::kWritev), iov_(iov), iovcnt_(iovcnt) {
}

void WritevContext::RegisterTo(Multiplexer* multiplexer) {
    auto ring = multiplexer->GetRing();
    auto sqe = ::io_uring_get_sqe(ring);
    assert(sqe != nullptr);

    ::io_uring_prep_writev(sqe, fd_, iov_, iovcnt_, 0);
    ::io_uring_sqe_set_data(sqe, this);
    auto submit_num = ::io_uring_submit(ring);
    assert(submit_num == 1);

    status_ = Status::kPolling;
}

ReadContext::ReadContext(Coroutine::Ptr co, int fd, void* buf, size_t count)
    : ReadvContext(co, fd, &iov_, 1), iov_{ buf, count } {
    event_ = Event::kRead;
}

void ReadContext::RegisterTo(Multiplexer* multiplexer) {
    ReadvContext::RegisterTo(multiplexer);
}

WriteContext::WriteContext(Coroutine::Ptr co, int fd, void* buf, size_t count)
    : WritevContext(co, fd, &iov_, 1), iov_{ buf, count } {
    event_ = Event::kWrite;
}

void WriteContext::RegisterTo(Multiplexer* multiplexer) {
    WritevContext::RegisterTo(multiplexer);
}

}