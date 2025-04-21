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
#include <mutex>

/**
 * @brief HighPrecisionTimer - A simple wrapper around Linux timerfd
 *        for executing a callback periodically in a dedicated thread.
 * 
 * Key features:
 * - Uses CLOCK_MONOTONIC to avoid time jumps
 * - Thread-safe start/stop using mutex and atomic flag
 * - Can elevate thread priority to real-time
 */
class HighPrecisionTimer {
public:
    using Callback = std::function<void()>;
    
    /**
     * @brief Constructor - Initializes timerfd.
     * 
     * Throws std::runtime_error if timerfd_create() fails.
     */
    explicit HighPrecisionTimer() {
        fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (fd == -1) {
            throw std::runtime_error("timerfd_create failed: " + std::string(strerror(errno)));
        }
    }

    /**
     * @brief Destructor - Stops the timer and cleans up resources.
     */
    ~HighPrecisionTimer() {
        stop();
        close(fd);
    }

    /**
     * @brief Starts the timer to trigger the callback every `millisecs` milliseconds.
     * 
     * If already running, it stops and restarts with the new interval.
     * 
     * @param millisecs Interval in milliseconds
     * @param cb Function to call on each timer expiration
     */
    void start(int millisecs, Callback cb);

    /**
     * @brief Stops the timer and joins the worker thread.
     */
    void stop();

private:
    int fd = -1;
    std::atomic<bool> running{false};
    std::thread worker;
    std::mutex timer_mutex;

    /**
     * @brief Internal loop that blocks on timerfd and triggers the callback.
     * 
     * @param cb User-provided function to call on each tick
     */
    void eventLoop(const Callback& cb);

    /**
     * @brief Optionally elevates thread priority using SCHED_FIFO.
     *        Logs a warning if elevation fails.
     */
    void setThreadPriority() ;
};

#endif