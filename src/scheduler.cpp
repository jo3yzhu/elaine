#include "scheduler.h"

namespace elaine
{

Scheduler::Scheduler(size_t thread_num, std::string name) : name_(name) {
    for (size_t i = 0; i < thread_num; ++i) {
        workers_.push_back(std::make_shared<Worker>(name_ + "_" + std::to_string(i)));
    }
    is_stopped_ = false;
}

Scheduler::~Scheduler() {
    if (!is_stopped_) {
        Stop();
    }
    assert(is_stopped_ == true);
}

void Scheduler::Start() {
    for (auto& worker : workers_) {
        worker->Start();
    }
    is_stopped_ = false;
}


void Scheduler::Stop() {
    for (auto& worker : workers_) {
        worker->Stop();
    }
    is_stopped_ = true;
}

void Scheduler::Wait() {
    for (auto& worker : workers_) {
        worker->Join();
    }
}

void Scheduler::AddTask(Coroutine::Ptr co) {
    auto worker = PickOneWorker();
    worker->AddTask(co);
}

Worker::Ptr Scheduler::PickOneWorker() {
    // simple round robin
    static size_t count = 0;
    auto worker = workers_[count++];
    if (count == workers_.size()) {
        count = 0;
    }
    return worker;
}

}