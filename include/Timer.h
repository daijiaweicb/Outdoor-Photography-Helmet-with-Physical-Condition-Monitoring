#ifndef TIMER_H
#define TIMER_H

#include <sys/timerfd.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

class HighPrecisionTimer {
public:
    using Callback = std::function<void()>;
    
    explicit HighPrecisionTimer() {
        fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (fd == -1) {
            throw std::runtime_error("timerfd_create failed: " + std::string(strerror(errno)));
        }
    }

    ~HighPrecisionTimer() {
        stop();
        close(fd);
    }

    void start(int millisecs, Callback cb);

    void stop();

private:
    int fd = -1;
    std::atomic<bool> running{false};
    std::thread worker;

    void eventLoop(const Callback& cb);

    void setThreadPriority() ;
};

#endif