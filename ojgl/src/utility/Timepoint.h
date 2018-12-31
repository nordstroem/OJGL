#pragma once
#include "Duration.h"

namespace ojgl {

class Timepoint {
public:
    explicit Timepoint(long timepoint)
        : _timepoint(timepoint)
    {
    }
    Timepoint()
        : _timepoint(0)
    {
    }
    Timepoint operator-(const Duration& other) { return Timepoint(_timepoint - other._duration); }
    Timepoint operator+(const Duration& other) { return Timepoint(_timepoint + other._duration); }
    Timepoint& operator-=(const Duration& other);
    Timepoint& operator+=(const Duration& other);

    static Timepoint now();
    friend Duration operator-(const Timepoint& left, const Timepoint& right) { return Duration(left._timepoint - right._timepoint); }

private:
    long _timepoint;
};
}
