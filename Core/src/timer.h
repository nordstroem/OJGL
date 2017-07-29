#pragma once

#include <chrono>

class Timer {

public:
    using clock_t = std::chrono::high_resolution_clock::time_point;
    using ms_t = std::chrono::milliseconds;
    using ns_t = std::chrono::nanoseconds;
    using s_t = std::chrono::seconds;
    using dsec_t = std::chrono::duration<double>;

    void start() { startTime = std::chrono::high_resolution_clock::now(); };
    void end() { endTime = std::chrono::high_resolution_clock::now(); };

    template <typename T>
    auto time() { return std::chrono::duration_cast<T>(startTime - endTime).count(); };

    template <typename T, typename Fun, typename... Args>
    static auto time(Fun&& f, Args&&... args)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        f(std::forward<Args>(args)...);
        auto dur = std::chrono::high_resolution_clock::now() - t1;
        return std::chrono::duration_cast<T>(dur).count();
    }

private:
    clock_t startTime, endTime;
};