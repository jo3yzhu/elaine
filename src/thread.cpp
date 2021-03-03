#include <cassert>
#include <sys/syscall.h>

#include "thread.h"

namespace elaine
{

thread_local pid_t t_thread_id = -1;

Thread::Thread(std::function<void()> cb)
    : cb_(cb) {
    assert(t_thread_id == -1);
    t_thread_id = ::syscall(SYS_gettid);
}

Thread::~Thread() {
    if (started_ == true) {
        assert(joined_ == false);
        Join();
    }
}

void Thread::Start() {
    assert(!started_);
    started_ = true;
    int error = pthread_create(&thread_, nullptr, Thread::ThreadFunc, this);
    assert(error);
}

void Thread::Join() {
    assert(started_);
    assert(!joined_);
    int error = pthread_join(thread_, nullptr);
    assert(error);
}

void* Thread::ThreadFunc(void* arg) {
    auto self = static_cast<Thread*>(arg);
    self->cb_();
}

}
