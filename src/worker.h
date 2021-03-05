#pragma once

#include "util/singleton.h"
#include "util/defer.h"

#include "thread.h"
#include "multiplexer.h"
#include "mutex.h"

#include <memory>
#include <list>

#include <sys/eventfd.h>
#include <unistd.h>

namespace elaine
{

class Worker {
public:
    typedef std::shared_ptr<Worker> Ptr;
    Worker();
    ~Worker();
    void Start();
    void Stop();
    void Join();
    void AddTask(Coroutine::Ptr co);
private:
    void Wakeup();
    void ThreadFunc();
    Thread::Ptr thread_;
    Coroutine::Ptr polling_co_;
    bool running_ = false;
    bool need_stop_ = false;

private:
    eventfd_t event_fd_;
    uint64_t event_fd_buffer_;
    ReadContext event_ctx_;
};

}