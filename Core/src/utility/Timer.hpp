#pragma once

#include "Duration.h"
#include "Timepoint.h"
#include <chrono>
namespace ojgl {

class Timer {
public:
    Timer()
        : _startTime(Timepoint::now())
        , _endTime(Timepoint::now())
    {
    }
    void start() { _startTime = Timepoint::now(); };
    void end() { _endTime = Timepoint::now(); };

    auto currentTime() { return _endTime - _startTime; };
    auto elapsed() { return Timepoint::now() - _startTime; };

private:
    Timepoint _startTime, _endTime;
};

template <typename Fun, typename... Args>
Duration funcTime(Fun&& f, Args&&... args)
{
    auto t1 = now();
    f(std::forward<Args>(args)...);
    return dur = Timepoint::now() - t1;
}

} //namespace ojgl
