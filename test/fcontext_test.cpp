#include "libcontext/libcontext.h"
#include <ucontext.h>

#include <stdlib.h>
#include <chrono>
#include <iostream>

fcontext_t main_fctx, co_fctx;
ucontext_t main_uctx, co_uctx;

void* spf, * spu;
size_t sp_size = 8192;
const int switch_times = 1000000;


void f_co(intptr_t) {
    for (int i = 0; i < switch_times; ++i) {
        jump_fcontext(&co_fctx, main_fctx, 0, false);
    }
}

void ftest() {
    spf = malloc(sp_size);
    co_fctx = make_fcontext(spf, sp_size, f_co);

    for (int i = 0; i < switch_times; ++i) {
        jump_fcontext(&main_fctx, co_fctx, 0, false);
    }
}

void u_co() {
    for (int i = 0; i < switch_times; ++i) {
        swapcontext(&co_uctx, &main_uctx);
    }
}

void utest() {
    spu = malloc(sp_size);
    getcontext(&co_uctx);
    co_uctx.uc_stack.ss_sp = spu;
    co_uctx.uc_stack.ss_size = sp_size;
    co_uctx.uc_link = NULL;

    makecontext(&co_uctx, u_co, 0);
    for (int i = 0; i < switch_times; ++i) {
        swapcontext(&main_uctx, &co_uctx);
    }
}

int main(int argc, char* argv[]) {

    auto t1 = std::chrono::system_clock::now();
    ftest();
    auto t2 = std::chrono::system_clock::now();
    auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "fcontext: " << double(d1.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << " second" << std::endl;

    utest();
    auto t3 = std::chrono::system_clock::now();
    auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
    std::cout << "ucontext: " << double(d2.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << " second" << std::endl;

    // ucontext is 10X slower compared with fcontext

    return 0;
}
