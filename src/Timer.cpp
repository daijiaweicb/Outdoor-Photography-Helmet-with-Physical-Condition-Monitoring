#include "Timer.h"
/**
 * @brief Starts the timer with the given interval and callback.
 * 
 * If already running, stops the previous timer first.
 * Spawns a worker thread that runs the callback at every interval.
 * 
 * @param millisecs Interval in milliseconds
 * @param cb        Callback function to invoke periodically
 */
void HighPrecisionTimer::start(int millisecs, Callback cb)
{
    // Ensure any previous timer is stopped
    stop();

    std::lock_guard<std::mutex> lock(timer_mutex);

    // Convert milliseconds to seconds and nanoseconds
    struct itimerspec its;
    const int sec = millisecs / 1000;
    const int nsec = (millisecs % 1000) * 1000000;

    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = nsec;
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = nsec;

    // Set timerfd with interval
    if (timerfd_settime(fd, 0, &its, nullptr) == -1)
    {
        throw std::runtime_error("timerfd_settime failed: " + std::string(strerror(errno)));
    }

    // Launch worker thread to handle timer events
    worker = std::thread([this, cb]() {
        setThreadPriority();
        eventLoop(cb);
    });

    running = true;
}

/**
 * @brief Stops the running timer and joins the worker thread.
 * 
 * Ensures thread-safe shutdown and avoids deadlocks by joining
 * outside the mutex lock scope.
 */
void HighPrecisionTimer::stop()
{
    std::thread localWorker;
    {
        std::lock_guard<std::mutex> lock(timer_mutex);
        if (!running)
            return;

        running = false;

        struct itimerspec its{};
        timerfd_settime(fd, 0, &its, nullptr);

        if (worker.joinable() && std::this_thread::get_id() != worker.get_id()) {
            localWorker = std::move(worker);
        } else {
            worker = std::thread();
        }
    }

    if (localWorker.joinable()) {
        localWorker.join();
    }
}

/**
 * @brief Main loop running in the timer thread.
 * 
 * Blocks on timerfd and calls the user callback each time
 * the timer expires. Will exit when `running` becomes false.
 * 
 * @param cb The user-supplied callback function
 */
void HighPrecisionTimer::eventLoop(const Callback &cb)
{
    constexpr size_t buf_size = sizeof(uint64_t);
    uint64_t exp;

    while (running)
    {
        ssize_t bytes = read(fd, &exp, buf_size);

        // signal interrupt
        if (bytes == -1 && errno == EINTR)
        {
            continue;
        }

        if (bytes != buf_size)
        {
            std::cerr << "Timer read error: " << strerror(errno) << std::endl;
            break;
        }

        // calback
        if (exp > 0 && cb)
        {
            try
            {
                cb();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Timer callback error: " << e.what() << std::endl;
            }
        }
    }
}

/**
 * @brief Attempts to set the thread priority to real-time (SCHED_FIFO).
 * 
 * Improves timing accuracy for timer callback execution. If it fails,
 * prints a warning but continues execution.
 */
void HighPrecisionTimer::setThreadPriority()
{
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);

    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0)
    {
        std::cerr << "Warning: Failed to set realtime priority ("
                  << strerror(errno) << ")" << std::endl;
    }
}