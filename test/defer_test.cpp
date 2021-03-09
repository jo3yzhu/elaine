#include "thread.h"
#include "mutex.h"
#include "util/defer.h"

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

using namespace elaine;

uint32_t g_counter = 0;
Mutex g_mutex;

int main(int argc, char* argv[]) {
    std::vector<Thread::Ptr> threads;
    for (int i = 0; i < 10; ++i) {
        auto t = std::make_shared<Thread>([&]() {
            g_mutex.Lock();
            defer([&]() {
                g_mutex.Unlock();
            });
            for (int j = 0; j < 10000; ++j) {
                ++g_counter;
            }
        });
        threads.push_back(t);
        t->Start();
    }

    for (auto& t : threads) {
        t->Join();
    }

    std::cout << g_counter << std::endl;
    //assert(g_counter == 1000);

    return 0;
}