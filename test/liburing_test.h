#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/timerfd.h>
#include <liburing.h>
#include <liburing/io_uring.h>

#include <chrono>

void env_support_check();
void timerfd_support_check();