#pragma once

#include <chrono>
#include <iostream>

namespace ojgl {

template <typename T>
using Duration = std::chrono::duration<long long, T>;

using Timepoint = std::chrono::high_resolution_clock::time_point;
using Milliseconds = std::chrono::milliseconds;
using Nanoseconds = std::chrono::nanoseconds;
using Seconds = std::chrono::seconds;

template <typename T, typename P>
T duration_cast(Duration<P> dur) { return std::chrono::duration_cast<T>(dur); }

Timepoint inline now() { return std::chrono::high_resolution_clock::now(); }

class Timer {
public:
    void start() { startTime = now(); };
    void end() { endTime = now(); };

    template <typename T>
    auto time() { return duration_cast<T>(endTime - startTime); };

    template <typename T>
    auto elapsed() { return duration_cast<T>(now() - startTime); };

private:
    Timepoint startTime, endTime;
};

template <typename T, typename Fun, typename... Args>
typename Duration<T>::rep funcTime(Fun&& f, Args&&... args)
{
    auto t1 = now();
    f(std::forward<Args>(args)...);
    auto dur = now() - t1;
    return duration_cast<T>(dur).count();
}

} //namespace ojgl
