#include "ITimer.h"
#include <thread>

/* Timer Event Generator implementing ITimer */

class TimerEventGenerator : public ITimer
{
    void registerTimer(const Timepoint &tp, const TTimerCallback &cb);

    void registerTimer(const Millisecs &period, const TTimerCallback &cb);

    void registerTimer(const Timepoint &tp, const Millisecs &period, const TTimerCallback &cb);

    void registerTimer(const TPredicate &pred, const Millisecs &period, const TTimerCallback &cb);
};


void TimerEventGenerator::registerTimer(const Timepoint &tp, const TTimerCallback &cb)
{

}


void TimerEventGenerator::registerTimer(const Millisecs &period, const TTimerCallback &cb)
{

}

void TimerEventGenerator::registerTimer(const Millisecs &period, const TTimerCallback &cb)
{

}

void TimerEventGenerator::registerTimer(const Millisecs &period, const TTimerCallback &cb)
{

}