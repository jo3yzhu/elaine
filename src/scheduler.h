#pragma once

#include "worker.h" 

#include <memory>
#include <vector>

namespace elaine
{

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> Ptr;
    Scheduler(size_t thread_num, std::string name);
    ~Scheduler();
    void Start();
    void Wait();
    void Stop(); // block call
    void AddTask(Coroutine::Ptr co);

private:
    Worker::Ptr PickOneWorker();
    std::vector<Worker::Ptr> workers_;
    bool is_stopped_ = true;
    std::string name_;
};

}