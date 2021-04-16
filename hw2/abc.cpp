#include "ITimer.hpp"

using CLOCK = std::chrono::high_resolution_clock;
using TTimerCallback = std::function<void()>;
static CLOCK::time_point T0;

void logCallback(int id, const std::string &logstr)
{
    auto dt = CLOCK::now() - T0;
    std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(dt).count()
              << "] (cb " << id << "): " << logstr << std::endl;
}

int main(int argc, char const *argv[])
{
    ITimer timer;
    T0 = CLOCK::now();

    logCallback(-1, "main starting.");
    auto t1 = CLOCK::now() + std::chrono::seconds(1);
    auto t2 = t1 + std::chrono::seconds(1);

    timer.registerTimer(t2, [&]() { logCallback(1, "callback str"); });
    timer.registerTimer(t1, [&]() { logCallback(2, "callback str"); });
    timer.registerTimer(t1, [&]() { logCallback(3, "callback str"); });


    timer.handler();

    // TODO. test with 3 others.

    return 0;
}
