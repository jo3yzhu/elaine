#include "thread.h"

#include <iostream>
#include <cassert>
#include <sys/syscall.h>


namespace elaine
{

thread_local pid_t t_thread_id = 0;

Thread::Thread(std::function<void()> cb)
    : cb_(cb) {
}

Thread::~Thread() {
    if (started_ && !joined_) {
        Join();
    }
}

void Thread::Start() {
    assert(!started_);
    started_ = true;
    int error = pthread_create(&thread_, nullptr, Thread::ThreadFunc, this);
    assert(error == 0);
}

void Thread::Join() {
    assert(started_);
    assert(!joined_);
    int error = pthread_join(thread_, nullptr);
    assert(error == 0);
    joined_ = true;
}

pid_t Thread::GetCurrentTid() {
    if (t_thread_id == 0) {
        
        t_thread_id = ::syscall(SYS_gettid);
    }
    return t_thread_id;
}


void* Thread::ThreadFunc(void* arg) {
    auto self = static_cast<Thread*>(arg);
    self->cb_();
    return nullptr;
}

}
