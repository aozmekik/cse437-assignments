#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <condition_variable>

using CLOCK = std::chrono::high_resolution_clock;
using TITimerCallback = std::function<void()>;
using Millisecs = std::chrono::milliseconds;
using Timepoint = CLOCK::time_point;
using TPredicate = std::function<bool()>;

class ITimer
{
public:
    // run the callback once at time point tp.
    void registerITimer(const Timepoint &tp, const TITimerCallback &cb);

    // run the callback periodically forever. The first call will be executed after the first period.
    void registerITimer(const Millisecs &period, const TITimerCallback &cb);

    // Run the callback periodically until time point tp. The first call will be executed after the first period.
    void registerITimer(const Timepoint &tp, const Millisecs &period, const TITimerCallback &cb);

    // Run the callback periodically. After calling the callback every time, call the predicate to check if the
    //termination criterion is satisfied. If the predicate returns false, stop calling the callback.
    void registerITimer(const TPredicate &pred, const Millisecs &period, const TITimerCallback &cb);

    ~ITimer();

private:
    class TimerItem
    {
    public:
        TimerItem();

        // TODO. getters

    private:
        TITimerCallback callback;
        TPredicate predict;
        Timepoint timepoint;
        Millisecs period;
    };

    void handler();
    bool run = true;

    std::mutex mtx;
    std::condition_variable cv;
};

void ITimer::registerITimer(const Timepoint &tp, const TITimerCallback &cb)
{
}

void ITimer::registerITimer(const Millisecs &period, const TITimerCallback &cb)
{
}

void ITimer::registerITimer(const Timepoint &tp, const Millisecs &period, const TITimerCallback &cb)
{
}

void ITimer::registerITimer(const TPredicate &pred, const Millisecs &period, const TITimerCallback &cb)
{
}

ITimer::TimerItem::TimerItem()
{
    // TODO. initialize section.
}

void ITimer::handler()
{
    std::unique_lock<std::mutex> lck(mtx);

    while (run)
    {
        cv.wait(lck);
    }
}

ITimer::~ITimer()
{
    std::unique_lock<std::mutex> lck(mtx);
    run = false;
    cv.notify_all();
}