#include "worker.h"

namespace elaine
{

Multiplexer* g_multiplexer = Singleton<Multiplexer>::GetInstance();

// TODO: support work stolen
std::list<Coroutine::Ptr> g_ready_coroutines; 
Mutex g_worker_mutex;

Worker::Worker()
    : event_fd_(eventfd(0, 0)),
    event_ctx_(polling_co_, event_fd_, &event_fd_buffer_, sizeof(event_fd_buffer_)) {
    thread_ = std::make_shared<Thread>(std::bind(&Worker::ThreadFunc, this));
    
    polling_co_ = std::make_shared<Coroutine>([&]() {
        while (true) {
            auto self = Coroutine::GetCurrent();
            auto ctx = g_multiplexer->Poll();
            if (ctx->GetStatus() == Context::Status::kSuccess) {
                auto co = ctx->GetCorouine(); // pointer of corouine may be nullptr because of wakeup
                if (co) {
                    AddTask(co);
                }
            }
            self->Yield();
        }
    });
    event_ctx_.RegisterTo(g_multiplexer);
}

Worker::~Worker() {
    Stop();
    ::close(event_fd_);
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
    if (!g_multiplexer->IsPolling()) {
        return;
    }

    uint64_t buffer = 1;
    ::write(event_fd_, &buffer, sizeof(buffer));
}

void Worker::ThreadFunc() {
    while (!need_stop_) {
        g_worker_mutex.Lock();
        if (g_ready_coroutines.empty()) {
            g_worker_mutex.Unlock();
            polling_co_->Resume();
        }
        else {
            auto co = g_ready_coroutines.front(); // TODO: load balance
            g_ready_coroutines.pop_front();
            g_worker_mutex.Unlock();
            co->Resume();
        }
    }
    running_ = false;
}

void Worker::AddTask(Coroutine::Ptr co) {
    if (g_multiplexer->IsPolling()) {
        Wakeup();
    }

    g_worker_mutex.Lock();
    defer([&]() {
        g_worker_mutex.Unlock();
    });
    g_ready_coroutines.push_back(std::move(co));
}


}