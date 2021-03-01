#include <memory>
#include <functional>
#include <cassert>
#include "libcontext/libcontext.h"

namespace elaine
{

class Coroutine : public std::enable_shared_from_this<Coroutine> {
public:
    enum class Status {
        kRunnable,      // initial
        kRunning,       // running non-blocking user logic
        kBlocked,       // wait for be scheduled
        kTerminated     // when member fn_ return
    };
    const static size_t kStackSize = 8192;
    typedef std::shared_ptr<Coroutine> Ptr;
public:
    Coroutine(std::function<void()> fn);
    Coroutine() = default;
    ~Coroutine();
    void Yield();
    void Resume();
    Status GetStatus();
    static Coroutine::Ptr GetActive();
    uint64_t GetGlobalCid();
    uint64_t GetThreadCid();
private:
    static void Run(intptr_t transfer);
    std::function<void()> fn_;
    fcontext_t ctx_;
    void* sp_ = nullptr;
    size_t sp_size_ = kStackSize;
    Status status_ = Status::kRunnable;
    uint64_t g_cid_;
    uint64_t t_cid_;
};

}
