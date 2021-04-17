#include "ITimer.hpp"

using CLOCK = std::chrono::high_resolution_clock;
using TTimerCallback = std::function<void()>;
static CLOCK::time_point T0;

// TODO. test with multi-threading, fix the design of cond-wait. it waits for no reason.

void logCallback(int id, const std::string &logstr)
{
    auto dt = CLOCK::now() - T0;
    std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(dt).count()
              << "] (cb " << id << "): " << logstr << std::endl;
}

int main(int argc, char const *argv[])
{

    ITimer timer;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    T0 = CLOCK::now();

    logCallback(-1, "main starting.");
    auto t1 = CLOCK::now() + std::chrono::seconds(1);
    auto t2 = t1 + std::chrono::seconds(1);


    timer.registerTimer(t2, [&]() { logCallback(1, "callback str"); });
    timer.registerTimer(t1, [&]() { logCallback(2, "callback str"); });

    timer.registerTimer(Millisecs(700), [&]() { logCallback(3, "callback str"); });
    timer.registerTimer(t1 + Millisecs(300), Millisecs(500), [&]() { logCallback(4, "callback str"); });

    timer.registerTimer([&]() {
        static int count = 0;
        return ++count < 3; }, Millisecs(500), [&]() { logCallback(5, "callback str"); });


    std::this_thread::sleep_for(std::chrono::seconds(5));
    logCallback(-1, "main terminating.");
    return 0;
}
