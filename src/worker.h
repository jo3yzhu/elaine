#pragma once

#include "util/singleton.h"
#include "util/defer.h"

#include "thread.h"
#include "multiplexer.h"
#include "mutex.h"

#include <memory>
#include <list>

namespace elaine
{

// A worker is a thread runs coroutines.
// The coroutine scheduling within a worker is non-preemptive, a coroutine can be resumed only if previous one yield itself.

// When a worker is freshly started, there's no coroutine in task queue and thread needs to resume poll coroutine to monitor occurring of event.
// Event could be triggered both by io_uring and by calling wakeup function manully:
// 1. by io_uring: poll coroutine add coroutine transferred by io_uring's user data to task queue and then yield itself.
// 2. manully: polling coroutine yield itself and just simply ignore io_uring's user data.
// Once polling coroutine yield itself, current thread starts to look around for newly coming corouine in task queue, resume to it if any


class Worker {
    friend class Multiplexer;
public:
    typedef std::shared_ptr<Worker> Ptr;
    Worker(std::string name);
    ~Worker();
    void Start();
    void Stop();
    void Join();
    void AddTask(Coroutine::Ptr co);
    std::string GetName();
    static Worker* GetCurrent();
private:
    void Wakeup();
    void Poll();
    void ThreadFunc();
    std::string name_;
    Thread::Ptr thread_;
    Multiplexer multiplexer_;
    bool running_ = false;
    bool need_stop_ = false;

private:
    Coroutine::Ptr polling_co_;
    NopContext nop_ctx_;
    std::list<Coroutine::Ptr> ready_coroutines_;
    Mutex mutex_;
};

}