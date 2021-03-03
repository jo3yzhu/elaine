#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <set>

#include "coroutine.h"

using namespace elaine;

size_t co_nums = 1000;
size_t co_loop_nums = 100;
size_t thread_nums = 4;

void thread_func() {
    size_t schedule_times = 0;
    std::set<Coroutine::Ptr> terminated_co;
    std::vector<Coroutine::Ptr> coroutines;
    for (size_t i = 0; i < co_nums; ++i) {
        coroutines.push_back(std::make_shared<Coroutine>([=]() {
            auto self = Coroutine::GetActive();
            for (size_t j = 0; j < co_loop_nums; ++j) {
                self->Yield();
            }
        }));
    }

    std::default_random_engine e;
    while (terminated_co.size() != co_nums) {
        // pick a coroutine randomly
        int index = e() % co_nums;
        auto co = coroutines[index];

        if (co->GetStatus() == Coroutine::Status::kTerminated) {
            if (terminated_co.count(co) == 0) {
                terminated_co.insert(co);
            }
        }
        else {
            co->Resume();
            schedule_times++;
        }

    }

    // entering coroutine needs one Resume call, Yield call match Resume call in fn_
    assert(schedule_times == (co_nums * (co_loop_nums + 1)));
}

int main(int argc, char* argv[]) {

    std::vector<std::thread> threads;
    for (size_t i = 0; i < thread_nums; ++i) {
        std::thread t(thread_func);
        threads.push_back(std::move(t));
    }

    for(auto &t : threads) {
        t.join();
    }

    return 0;
}