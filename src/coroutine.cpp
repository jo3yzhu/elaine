#include <atomic>
#include <iostream>
#include "coroutine.h"

namespace elaine
{

thread_local fcontext_t t_main_ctx;               // context for main coroutine
thread_local Coroutine* t_active_co = nullptr;    // nullptr indicates that main coroutine is active
thread_local uint64_t t_cid_generator = 0;        // for thread local coroutine id generating
std::atomic<uint64_t> g_cid_generator = { 0 };    // global atomic for coroutine id generating

Coroutine::Coroutine(std::function<void()> fn)
    : fn_(fn) {
    // set global and thread local coroutine id
    g_cid_ = g_cid_generator++;
    t_cid_ = t_cid_generator++;

    sp_ = malloc(sp_size_);

    //auto sp = reinterpret_cast<void*>(((ptrdiff_t)sp_ + sp_size_ - 0xf) & (~0x0f)); // memory should be 16 bytes aligned
    //sp_size_ -= ((ptrdiff_t)sp_ - (ptrdiff_t)sp + sp_size_); // adjust stack size after aligned
    
    auto sp = reinterpret_cast<void*>((ptrdiff_t)sp_ + sp_size_);
    ctx_ = make_fcontext(sp, sp_size_, Run);
    status_ = Status::kRunnable;
}

Coroutine::~Coroutine() {
    if (sp_) {
        free(sp_);
    }

    // TODO: use log library
    // std::cout << "coroutine: " << g_cid_ << "destructed";
}

// @description: switch control stream of current thread to a io corouine
// @note: must be called in main coroutine of current thread

void Coroutine::Resume() {
    assert(status_ != Status::kRunning);
    t_active_co = this;
    status_ = Status::kRunning;
    jump_fcontext(&t_main_ctx, ctx_, 0, false);
}

// @description: switch control stream of current thread to thread local main coroutine
// @note: must be called in io coroutine of current thread

void Coroutine::Yield() {
    assert(status_ != Status::kBlocked);
    t_active_co = nullptr;
    status_ = Status::kBlocked;
    jump_fcontext(&ctx_, t_main_ctx, 0, false);
}

Coroutine::Status Coroutine::GetStatus() {
    return status_;
}

// @description: get active coroutine in current thread

Coroutine::Ptr Coroutine::GetActive() {
    assert(t_active_co != nullptr);
    return t_active_co->shared_from_this();
}

uint64_t Coroutine::GetGlobalCid() {
    return g_cid_;
}

uint64_t Coroutine::GetThreadCid() {
    return t_cid_;
}

void Coroutine::Run(intptr_t transfer) {
    auto self = t_active_co;
    self->fn_();
    self->fn_ = nullptr;
    self->status_ = Status::kTerminated;

    jump_fcontext(&self->ctx_, t_main_ctx, 0, false);

    assert(false); // should never reach here
}


}