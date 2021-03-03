#pragma once

#include <atomic>

#include <liburing.h>
#include <liburing/io_uring.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "context.h"

namespace elaine
{

// multiplexer class is a global singleton and thread-safe

class Multiplexer {
public:
    Multiplexer(unsigned int depth);
    Multiplexer(const Multiplexer&) = delete;
    Multiplexer(Multiplexer&&) = delete;
    ~Multiplexer();

    struct io_uring* GetRing();

    template <typename T>
    void RegisterContext(T ctx_) {
        ctx_->RegisterSelf();
    }

    Context::Ptr Poll();
    void Wakeup();

private:
    struct io_uring ring_;
    unsigned int queue_depth_;
    std::atomic<bool> is_polling_;

    eventfd_t event_fd_;
    EventContext::Ptr event_ctx_;
};

}
