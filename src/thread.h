#pragma once

#include <functional>
#include <memory>

#include <unistd.h>
#include <pthread.h>

namespace elaine
{

class Thread {
public:
    typedef std::shared_ptr<Thread> Ptr;
public:
    Thread(std::function<void()> cb);
    ~Thread();
    void Start();
    void Join();
    static pid_t GetCurrentTid();
private:
    static void* ThreadFunc(void* arg);
    pthread_t thread_;
    bool started_ = false;
    bool joined_ = false;
    std::function<void()> cb_;
};

}
