#include <iostream>
#include <type_traits>
#include <string>
#include <cstdint>

#define DEF_IS(check_type, return_type) \
    template<class T> \
    typename std::enable_if<std::is_same<T, check_type>::value, return_type>::type

// supported i/o event
enum class Event : uint32_t {
    kNone,
    kAccept,
    kReadv,
    kWritev,
    kRead,
    kWrite,
};

// some status can be set in context, but some can only be set in worker
enum class Status : uint32_t {
    kInitial,
    kPolling,
    kSuccess,
    kFail,
};

DEF_IS(Event, std::string) ToString(T t) {
    switch (t) {
    case Event::kNone:
        return "None";
    case Event::kAccept:
        return "Accept";
    case Event::kReadv:
        return "Readv";
    case Event::kWritev:
        return "Writev";
    case Event::kRead:
        return "Read";
    case Event::kWrite:
        return "Write";
    default:
        return "Unknown";
    }
}

DEF_IS(Status, std::string) ToString(T t) {
    switch (t) {
    case Status::kInitial:
        return "Initial";
    case Status::kPolling:
        return "Polling";
    case Status::kSuccess:
        return "Success";
    case Status::kFail:
        return "Fail";
    default:
        return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    std::cout << ToString<Event>(Event::kAccept) << std::endl;
    std::cout << ToString(Status::kInitial) << std::endl;
    return 0;
}