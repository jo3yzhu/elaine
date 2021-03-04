#pragma once

#include <pthread.h>

namespace elaine
{

class Mutex {
public:
    Mutex();
    ~Mutex();
    void Lock();
    void Unlock();
private:
    pthread_mutex_t mutex_;
};

}
