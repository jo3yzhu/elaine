#pragma once

#include <memory>

#include <liburing.h>
#include <liburing/io_uring.h>

#include "coroutine.h"


namespace elaine
{

class Multiplexer;

// context class handles the life-time context of a coroutine
// a context object should be transfered between user and kernel by io_uring to specify context of io_uring event just return
// the smart pointer of context obejcts would be managed in a high performace key-value container for indexing by fd
// there cannot be multithreads own one context, so it's thread-safe
// TODO: support cancel

class Context : public std::enable_shared_from_this<Context> {
    friend class Multiplexer;
public:
    typedef std::shared_ptr<Context> Ptr;
    enum class Event : uint32_t {
        kNone = 0x00,
        kAccept = 0x01,
        kRead = 0x02,
        kWrite = 0x04,
        kEvent = 0x08,
    };

    // some status can be set in context, but some can only be set in multipexer
    enum class Status : uint32_t {
        kInitial = 0x00,
        kPolling = 0x01,
        kSuccess = 0x02, // if return value of specified system call is valid, status is set as success
        kFail = 0x04,    // if not, status is set as fail
    };

public:
    explicit Context(Multiplexer* multiplexer, Coroutine::Ptr co, int fd, Event listen_events);
    virtual void RegisterSelf() = 0; // context must know how to register itself in multiplexer
    virtual void CheckOut() = 0;     // called when multiplexer return, set result of system and context status

    Coroutine::Ptr GetCorouine();
    Event GetEvent();
    Status GetStatus();

protected:
    Multiplexer* multiplexer_ = nullptr; // always alive, no need for shared_ptr and avoiding circular reference
    Coroutine::Ptr co_ = nullptr;
    int fd_;
    int res_; // result of asynchronous system call, should be set by multiplexer
    Event event_ = Event::kNone; // interested event
    Status status_ = Status::kInitial;
};

class EventContext : public Context {
public:
    typedef std::shared_ptr<EventContext> Ptr;
    explicit EventContext(Multiplexer* multiplexer, Coroutine::Ptr co, int fd);
    void RegisterSelf() override;
    void CheckOut() override;
private:
    uint64_t buffer_;
};

class AcceptContext : public Context {
public:
    typedef std::shared_ptr<AcceptContext> Ptr;
    explicit AcceptContext(Multiplexer* multiplexer, Coroutine::Ptr co, int fd);
    void RegisterSelf() override;
    void CheckOut() override;
private:
    sockaddr peer_addr_; // TODO: use user-defined socket instance
};


}