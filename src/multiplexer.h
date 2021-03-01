#include <liburing.h>
#include <liburing/io_uring.h>

namespace elaine
{

class Multiplexer {
public:
    enum class Event {
        kAccept,
        kRead,
        kWrite,
    };
public:
    Multiplexer(int depth);
    Multiplexer(const Multiplexer&) = delete;
    Multiplexer(Multiplexer &&) = delete;

private:
    struct io_uring ring_;
    int ring_depth_;
};

}
