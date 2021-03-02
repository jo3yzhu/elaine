#include <memory>
#include <mutex>

namespace elaine
{

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
