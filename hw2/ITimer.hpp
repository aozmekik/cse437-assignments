#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cassert>
#include <algorithm>

using CLOCK = std::chrono::high_resolution_clock;
using TITimerCallback = std::function<void()>;
using Millisecs = std::chrono::milliseconds;
using Timepoint = CLOCK::time_point;
using TPredicate = std::function<bool()>;

class ITimer
{
public:
    // run the callback once at time point tp.
    void registerTimer(const Timepoint &tp, const TITimerCallback &cb);

    // run the callback periodically forever. The first call will be executed after the first period.
    void registerTimer(const Millisecs &period, const TITimerCallback &cb);

    // Run the callback periodically until time point tp. The first call will be executed after the first period.
    void registerTimer(const Timepoint &tp, const Millisecs &period, const TITimerCallback &cb);

    // Run the callback periodically. After calling the callback every time, call the predicate to check if the
    //termination criterion is satisfied. If the predicate returns false, stop calling the callback.
    void registerTimer(const TPredicate &pred, const Millisecs &period, const TITimerCallback &cb);

    ~ITimer();

    void handler();

private:
    class TimerItem
    {
    public:
        TimerItem(const TITimerCallback &, const TPredicate &, const Millisecs &);

        const TITimerCallback &getCallBack() const;
        bool getPredict() const;
        const Millisecs &getPeriod() const;

        Millisecs getTimeToWait() const;
        void setTimeToWait(Millisecs);

    private:
        TITimerCallback callback;
        TPredicate predict;
        Millisecs period;
        Millisecs time_to_wait;
    };

    // gets the next item from the vector with the highest priority
    TimerItem &next();
    void wait(TimerItem &);
    void execute(TimerItem &);

    bool run = true;

    std::mutex mtx;
    std::condition_variable cv;

    std::vector<TimerItem> list;

    const static Millisecs zero_sec;
};

const Millisecs ITimer::zero_sec = Millisecs(0);

void ITimer::registerTimer(const Timepoint &tp, const TITimerCallback &cb)
{
    std::unique_lock<std::mutex> lck(mtx);

    Millisecs ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - CLOCK::now());
    // auto t1 = std::chrono::seconds(0);
    // auto t2 = tp + std::chrono::seconds(0);

    list.push_back(TimerItem(
        cb, [tp]() { 
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(CLOCK::now() - tp).count();
            // std:: cout << diff << std::endl;
            // std::cout << (diff < 0) << std::endl;
            return (diff < 0); }, ms));

    cv.notify_all();
}

void ITimer::registerTimer(const Millisecs &period, const TITimerCallback &cb)
{
    std::unique_lock<std::mutex> lck(mtx);

    list.push_back(TimerItem(
        cb, []() { return true; }, period));

    cv.notify_all();
}

void ITimer::registerTimer(const Timepoint &tp, const Millisecs &period, const TITimerCallback &cb)
{
    std::unique_lock<std::mutex> lck(mtx);

    list.push_back(TimerItem(
        cb, [tp]() { return CLOCK::now() < tp; }, period));

    cv.notify_all();
}

void ITimer::registerTimer(const TPredicate &pred, const Millisecs &period, const TITimerCallback &cb)
{
    std::unique_lock<std::mutex> lck(mtx);

    list.push_back(TimerItem(cb, pred, period));

    cv.notify_all();
}

void ITimer::handler()
{
    std::unique_lock<std::mutex> lck(mtx);

    while (run)
    {
        if (list.empty())
            break;

        // get item - wait for item if necessary - execute
        auto &item = next();
        wait(item);
        execute(item);

        cv.wait(lck);
    }
}

ITimer::TimerItem &ITimer::next()
{
    // FIXME. if list is empty?

    // sort the list in the descending order, last item has the highest priority.
    std::sort(list.begin(), list.end(), [](const TimerItem &lhs, const TimerItem &rhs) {
        return lhs.getTimeToWait() > rhs.getTimeToWait();
    });

    // get the item with the highest priority
    return list.back();
}

void ITimer::wait(TimerItem &item)
{
    // auto &elapsed = item.getElapsed();
    auto time_to_wait = item.getTimeToWait();

    if (time_to_wait > zero_sec)
    {
        std::this_thread::sleep_for(time_to_wait);

        // update other items time to wait.
        std::for_each(list.begin(), list.end(), [&time_to_wait](TimerItem &ti) { ti.setTimeToWait(ti.getTimeToWait() - time_to_wait); });
    }

    // refresh time to wait
    item.setTimeToWait(item.getPeriod());

    if (!item.getPredict())
        list.pop_back();
}

void ITimer::execute(TimerItem &item)
{
    std::function<void()> f = item.getCallBack();
    f();
}

ITimer::~ITimer()
{
    std::unique_lock<std::mutex> lck(mtx);
    run = false;
    cv.notify_all();
}

ITimer::TimerItem::TimerItem(const TITimerCallback &cb, const TPredicate &pred, const Millisecs &ms)
    : callback(cb), predict(pred), period(ms), time_to_wait(ms)
{
    /* intentionally left blank */
    // std::cout << time_to_wait.count() << std::endl;
}

inline const TITimerCallback &ITimer::TimerItem::getCallBack() const
{
    return callback;
}
inline bool ITimer::TimerItem::getPredict() const
{
    return predict();
}
inline const Millisecs &ITimer::TimerItem::getPeriod() const
{
    return period;
}

inline Millisecs ITimer::TimerItem::getTimeToWait() const
{
    return time_to_wait;
}

void ITimer::TimerItem::setTimeToWait(Millisecs ms)
{
    // FIXME. could Milliseconds type go below zero?
    time_to_wait = ms;
}