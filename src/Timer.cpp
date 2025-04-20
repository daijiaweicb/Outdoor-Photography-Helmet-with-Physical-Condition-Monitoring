#include "Timer.h"
/**
 * HighPrecisionTimer. start the timer
 * 
 * @param  {int} millisecs : 
 * @param  {Callback} cb   : 
 */
void HighPrecisionTimer::start(int millisecs, Callback cb) {
    if (running.exchange(true)) return;

    struct itimerspec its;
    const int sec = millisecs / 1000;
    const int nsec = (millisecs % 1000) * 1000000;

    //Calculate cycle interval
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = nsec;
    its.it_interval.tv_sec = sec;    
    its.it_interval.tv_nsec = nsec;

    if (timerfd_settime(fd, 0, &its, nullptr) == -1) {
        throw std::runtime_error("timerfd_settime failed: " + std::string(strerror(errno)));
    }

    //Start woker thread
    worker = std::thread([this, cb]() {
        setThreadPriority();
        eventLoop(cb);
    });
}

/**
 * Stop the timer
 */
void HighPrecisionTimer::stop() 
{
    if (!running.exchange(false)) return;
    
    struct itimerspec its{};
    timerfd_settime(fd, 0, &its, nullptr);
    
    if (worker.joinable()) {
        worker.join();
    }
}
/**
 * HighPrecisionTimer 
 * 
 * @param  {Callback} cb : 
 */
void HighPrecisionTimer:: eventLoop(const Callback& cb) {
    constexpr size_t buf_size = sizeof(uint64_t);
    uint64_t exp;

    while (running) {
        ssize_t bytes = read(fd, &exp, buf_size);
        
        // signal interrupt
        if (bytes == -1 && errno == EINTR) {
            continue;
        }

        if (bytes != buf_size) {
            std::cerr << "Timer read error: " << strerror(errno) << std::endl;
            break;
        }

        // calback
        if (exp > 0 && cb) {
            try {
                cb();
            } catch (const std::exception& e) {
                std::cerr << "Timer callback error: " << e.what() << std::endl;
            }
        }
    }
}
/**
 * Setting thread priority
 */
void HighPrecisionTimer:: setThreadPriority() {
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0) {
        std::cerr << "Warning: Failed to set realtime priority (" 
                  << strerror(errno) << ")" << std::endl;
    }
}