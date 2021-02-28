#include "related_research.h"


static const char* op_strs[] = {
    "IORING_OP_NOP",
    "IORING_OP_READV",
    "IORING_OP_WRITEV",
    "IORING_OP_FSYNC",
    "IORING_OP_READ_FIXED",
    "IORING_OP_WRITE_FIXED",
    "IORING_OP_POLL_ADD",
    "IORING_OP_POLL_REMOVE",
    "IORING_OP_SYNC_FILE_RANGE",
    "IORING_OP_SENDMSG",
    "IORING_OP_RECVMSG",
    "IORING_OP_TIMEOUT",
    "IORING_OP_TIMEOUT_REMOVE",
    "IORING_OP_ACCEPT",
    "IORING_OP_ASYNC_CANCEL",
    "IORING_OP_LINK_TIMEOUT",
    "IORING_OP_CONNECT",
    "IORING_OP_FALLOCATE",
    "IORING_OP_OPENAT",
    "IORING_OP_CLOSE",
    "IORING_OP_FILES_UPDATE",
    "IORING_OP_STATX",
    "IORING_OP_READ",
    "IORING_OP_WRITE",
    "IORING_OP_FADVISE",
    "IORING_OP_MADVISE",
    "IORING_OP_SEND",
    "IORING_OP_RECV",
    "IORING_OP_OPENAT2",
    "IORING_OP_EPOLL_CTL",
    "IORING_OP_SPLICE",
    "IORING_OP_PROVIDE_BUFFERS",
    "IORING_OP_REMOVE_BUFFERS",
    "IORING_OP_TEE",
    "IORING_OP_SHUTDOWN",
    "IORING_OP_RENAMEAT",
    "IORING_OP_UNLINKAT",
    "IORING_OP_MKDIRAT",
};

void env_support_check() {
    struct utsname u;
    uname(&u);
    printf("You are running kernel version: %s\n", u.release);
    struct io_uring_probe* probe = io_uring_get_probe();
    printf("Report of your kernel's list of supported io_uring operations:\n");
    for (char i = 0; i < IORING_OP_LAST; i++) {
        printf("%s: ", op_strs[i]);
        if (io_uring_opcode_supported(probe, i))
            printf("yes.\n");
        else
            printf("no.\n");
    }
    free(probe);
}

void timerfd_support_check() {
    int timer_internal_second = 1;
    int timer_expired_times = 10;
    int timer_expired_seq = 0;
    int timer_unique_id[] = { 0,1,2,3,4,5,6,7,8,9 };

    // init timerfd, timerfd must be in block mode when work with io_uring
    int tfd = timerfd_create(CLOCK_REALTIME, 0);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // set timer internal and firstly expired time
    struct itimerspec new_value;
    new_value.it_value.tv_sec = now.tv_sec + timer_internal_second;
    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_interval.tv_sec = timer_internal_second;
    new_value.it_interval.tv_nsec = 0;

    // init io_uring
    struct io_uring ring;
    io_uring_queue_init(1, &ring, 0);
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);

    // submit read request to io_uring
    uint64_t expired_times = 0;
    io_uring_prep_read(sqe, tfd, &expired_times, sizeof(uint64_t), 0);
    io_uring_sqe_set_data(sqe, &timer_unique_id[timer_expired_seq++]);
    io_uring_submit(&ring);

    // before timer start
    auto before = std::chrono::system_clock::now().time_since_epoch();
    auto before_ms = std::chrono::duration_cast<std::chrono::microseconds>(before);
    printf("timer firstly started at: %ld\n", before_ms.count());

    // start timer
    timerfd_settime(tfd, TFD_TIMER_ABSTIME, &new_value, NULL);

    // wait for expired timer
    for (int count = 0; count < timer_expired_times; ++count) {
        struct io_uring_cqe* cqe;
        io_uring_wait_cqe(&ring, &cqe);
        int seq = *(int*)io_uring_cqe_get_data(cqe);
        
        // cqe must be seen, if not, the result of io_uring_wait_cqe would always be the head node of CQ and seems like never be removed
        io_uring_cqe_seen(&ring, cqe); 

        auto after = std::chrono::system_clock::now().time_since_epoch();
        auto after_ms = std::chrono::duration_cast<std::chrono::microseconds>(after);
        printf("timer exipred at: %ld and seq is: %d\n", after.count(), seq);

        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        io_uring_prep_read(sqe, tfd, &expired_times, sizeof(uint64_t), 0);
        io_uring_sqe_set_data(sqe, &timer_unique_id[timer_expired_seq++]);
        io_uring_submit(&ring);
    }
}