#pragma once

#include "coroutine.h"

#include <liburing.h>
#include <liburing/io_uring.h>

namespace elaine
{


// Design:
// 1. context class handles the life-time context of a coroutine and is only allowed to ve allocated on coroutine stack
// 2. a context object should be transfered between user and kernel by io_uring user data to specify current context
// 3. there cannot be multithreads own one context, so it's thread-safe

// Note:
// 1. io_uring_prep_recv/read/write just would not work with socket on my machine: Linux ubuntu 5.8.0-44-generic #50~20.04.1-Ubuntu SMP Thu Feb 11 07:01:18 UTC 2021 aarch64 aarch64 aarch64 GNU/Linux
// 2. io_uring_prep_readv/writev can be used with both socket and file, so I plan implement asynchronous read/write/recv/send based on readv/writev

class Multiplexer;

class Context {
    friend class Multiplexer;
public:

    // supported i/o event
    enum class Event : uint32_t {
        kNone,
        kAccept,
        kReadv,
        kWritev,
        kRead,
        kWrite,
    };

    // some status can be set in context, but some can only be set in worker
    enum class Status : uint32_t {
        kInitial,
        kPolling,
        kSuccess,
        kFail,
    };

public:
    Context(Coroutine::Ptr co, int fd, Event listen_events);

    // context must know how to register itself in multiplexer
    virtual void RegisterTo(Multiplexer* multiplexer) = 0;

    // avoid be allocated in heap
    void* operator new(size_t) throw() = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;

    // getter
    Coroutine::Ptr GetCorouine();
    int GetResult();
    int GetFd();
    Event GetEvent();
    Status GetStatus();

protected:
    // when i/o finished, worker will just call Resume() of it
    Coroutine::Ptr co_ = nullptr;

    // the file need to read or write etc
    int fd_;

    // result of asynchronous system call, should be set by multiplexer
    ssize_t res_;

    // interested event
    Event event_ = Event::kNone;

    // status of io_uring function call
    Status status_ = Status::kInitial;
};


class AcceptContext : public Context {
public:
    AcceptContext(Coroutine::Ptr co, int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    void RegisterTo(Multiplexer* multiplexer) override;
    sockaddr* addr_;
    socklen_t* addrlen_;
};

class ReadvContext : public Context {
public:
    ReadvContext(Coroutine::Ptr co, int fd, struct iovec* iov, int iovcnt);
    void RegisterTo(Multiplexer* multiplexer) override;
    const struct iovec* iov_;
    int iovcnt_;
};

class WritevContext : public Context {
public:
    WritevContext(Coroutine::Ptr co, int fd, const struct iovec* iov, int iovcnt);
    void RegisterTo(Multiplexer* multiplexer) override;
    const struct iovec* iov_;
    int iovcnt_;
};

class ReadContext : public ReadvContext {
public:
    ReadContext(Coroutine::Ptr co, int fd, void* buf, size_t count);
    void RegisterTo(Multiplexer* multiplexer) override;
    struct iovec iov_;
};

class WriteContext : public WritevContext {
public:
    WriteContext(Coroutine::Ptr co, int fd, void* buf, size_t count);
    void RegisterTo(Multiplexer* multiplexer) override;
    struct iovec iov_;
};


}