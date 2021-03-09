#pragma once

#include <atomic>

#include <liburing.h>
#include <liburing/io_uring.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "util/defer.h"
#include "context.h"

namespace elaine
{


// multiplexer is a wrapper of event register and polling implemented by io_uring which is singleton
// public interface of multiplexer is thread-safe

class Multiplexer {
public:
    static constexpr size_t DefaultRingDepth = 1024;
public:
    Multiplexer();
    Multiplexer(const Multiplexer&) = delete;
    Multiplexer(Multiplexer&&) = delete;
    ~Multiplexer();

    static void SetCurrent(Multiplexer *multiplexer);
    static Multiplexer* GetCurrent();
    struct io_uring* GetRing();

    template <typename T>
    void RegisterContext(T ctx_) {
        ctx_->RegisterTo(this);
    }

    Context* Poll();
    bool IsPolling();

private:
    struct io_uring ring_;
    unsigned int queue_depth_;
    std::atomic<bool> is_polling_;
};

}
