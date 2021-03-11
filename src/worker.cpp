#include "worker.h"

#include <iostream>

namespace elaine
{


// TODO: support work stolen instead of global task queue
// std::list<Coroutine::Ptr> g_ready_coroutines;
// Mutex g_worker_mutex;

thread_local Worker* t_worker = nullptr;

Worker::Worker(std::string name)
    : name_(name),
    polling_co_(std::make_shared<Coroutine>(std::bind(&Worker::Poll, this), "pollingco")) {
    thread_ = std::make_shared<Thread>(std::bind(&Worker::ThreadFunc, this));
    nop_ctx_.RegisterTo(&multiplexer_);
}

Worker::~Worker() {
    Stop();
}

void Worker::Start() {
    running_ = true;
    thread_->Start();
}

void Worker::Stop() {
    need_stop_ = true;
    thread_->Join();
}

void Worker::Join() {
    thread_->Join();
}

void Worker::Wakeup() {
    nop_ctx_.RegisterTo(&multiplexer_);
}

std::string Worker::GetName() {
    return name_;
}

Worker* Worker::GetCurrent() {
    return t_worker;
}

void Worker::Poll() {
    while (true) {
        auto self = Coroutine::GetCurrent();
        auto ctx = multiplexer_.Poll(); // return ctx is nullptr when wakeup is triggered by nop context
        if (ctx && ctx->GetStatus() == Context::Status::kSuccess) {
            auto co = ctx->GetCorouine(); // pointer of corouine may be nullptr because of wakeup
            AddTask(co);
        }

        self->Yield();
    }
}

void Worker::ThreadFunc() {
    t_worker = this;
    Multiplexer::SetCurrent(&multiplexer_);
    while (!need_stop_) {
        mutex_.Lock();

        if (ready_coroutines_.empty()) {
            mutex_.Unlock();
            polling_co_->Resume();
        }
        else {
            auto co = ready_coroutines_.front();
            ready_coroutines_.pop_front();
            mutex_.Unlock();
            co->Resume();
        }


    }
    running_ = false;
}

void Worker::AddTask(Coroutine::Ptr co) {
    mutex_.Lock();
    defer([&]() {
        mutex_.Unlock();
    });

    ready_coroutines_.push_back(co);

    if (multiplexer_.IsPolling()) {
        Wakeup();
    }
}


}