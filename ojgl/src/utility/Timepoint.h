#pragma once
#include "Duration.h"
#include <Windows.h>
#include <chrono>

namespace ojgl {

class Timepoint {

    using Backend = DWORD;

    explicit Timepoint(Backend timepoint)
        : _timepoint(timepoint)
    {
    }

public:
    Timepoint operator-(const Duration& other) { return Timepoint(_timepoint - other._duration); }
    Timepoint operator+(const Duration& other) { return Timepoint(_timepoint + other._duration); }
    Timepoint& operator-=(const Duration& other);
    Timepoint& operator+=(const Duration& other);

    inline static auto now() { return Timepoint(GetTickCount()); }

    friend Duration operator-(const Timepoint& left, const Timepoint& right) { return Duration(left._timepoint - right._timepoint); }

private:
    Backend _timepoint;
};
}
