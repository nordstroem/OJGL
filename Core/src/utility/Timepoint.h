#pragma once
#include "Duration.h"
#include <chrono>

namespace ojgl {

class Timepoint {

    using Backend = std::chrono::high_resolution_clock;

    explicit Timepoint(const Backend::time_point& timepoint)
        : _timepoint(timepoint)
    {
    }

public:
    Timepoint operator-(const Duration& other) { return Timepoint(_timepoint - other._duration); }
    Timepoint& operator-=(const Duration& other);
    Timepoint& operator+=(const Duration& other);

    inline static auto now() { return Timepoint(Backend::now()); }

    friend Duration operator-(const Timepoint& left, const Timepoint& right) { return Duration(left._timepoint - right._timepoint); }

private:
    Backend::time_point _timepoint;
};
}
