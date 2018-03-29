#pragma once

#include <chrono>

namespace ojgl {

namespace timer {
    template <typename T>
    using Duration = std::chrono::duration<long long, T>;

    using Timepoint = std::chrono::high_resolution_clock::time_point;
    using Milliseconds = std::chrono::milliseconds;
    using Nanoseconds = std::chrono::nanoseconds;
    using Seconds = std::chrono::seconds;

    class Timer {
    public:
        void start() { startTime = std::chrono::high_resolution_clock::now(); };
        void end() { endTime = std::chrono::high_resolution_clock::now(); };

        template <typename T>
        auto time() { return std::chrono::duration_cast<T>(endTime - startTime); };

        template <typename T>
        auto elapsed() { return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - startTime); };

    private:
        Timepoint startTime, endTime;
    };

    template <typename T, typename Fun, typename... Args>
    typename Duration<T>::rep funcTime(Fun&& f, Args&&... args)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        f(std::forward<Args>(args)...);
        auto dur = std::chrono::high_resolution_clock::now() - t1;
        return std::chrono::duration_cast<T>(dur).count();
    }

    template <typename T, typename P>
    T duration_cast(Duration<P> dur) { return std::chrono::duration_cast<T>(dur); }

    Timepoint inline now() { return std::chrono::high_resolution_clock::now(); }

} //namespace timer
} //namespace ojgl
