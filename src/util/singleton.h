#pragma once

#include <memory>
#include <mutex>

namespace elaine
{

// a simple version of singleton wrapper, for there can only be a instance per class type
// if there are many instance getting called with different params within one class, only first call can be in effect

template <typename T>
class Singleton {
public:
    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    template<typename... Args>
    static T* GetInstance(Args... args) {
        if (!instance_) {
            std::call_once(once_, [&]() {
                instance_ = new T(args...);
            });
        }

        return instance_;
    }

private:
    static T* instance_;
    static std::once_flag once_;
};

template <typename T>
std::once_flag Singleton<T>::once_{};

template <typename T>
T* Singleton<T>::instance_{nullptr};

}
