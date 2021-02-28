#include <unistd.h>
#include <sys/timerfd.h>

#include <liburing.h>
#include <liburing/io_uring.h>

#include "related_research.h"


int main(int argc, char *argv[]) {
    timerfd_support_check();
    return 0;
}