#pragma once

#include <chrono>
#include <ostream>

namespace ojgl {

class Duration {

    friend class Timepoint;

    using Backend = std::chrono::nanoseconds;

public:
    Duration(const Backend& duration)
        : _duration(duration)
    {
    }

    Duration() = default;

    long long toSeconds() const { return std::chrono::duration_cast<std::chrono::seconds>(_duration).count(); }
    long long toMilliseconds() const { return std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count(); }
    long long toNanoseconds() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(_duration).count(); }

    Duration operator-() const { return Duration(-this->_duration); }
    Duration operator-(const Duration& other) const { return Duration(this->_duration - other._duration); }
    Duration operator+(const Duration& other) const { return Duration(this->_duration + other._duration); }
    bool operator<(const Duration& other) const { return this->_duration < other._duration; }
    bool operator<=(const Duration& other) const { return this->_duration <= other._duration; }
    bool operator>(const Duration& other) const { return this->_duration > other._duration; }
    bool operator>=(const Duration& other) const { return this->_duration >= other._duration; }
    bool operator==(const Duration& other) const { return this->_duration == other._duration; }

    Duration& operator-=(const Duration& other);
    Duration& operator+=(const Duration& other);

    friend std::ostream& operator<<(std::ostream& left, const Duration& right);

public:
    static Duration seconds(long long s) { return Duration(std::chrono::seconds(s)); }
    static Duration milliseconds(long long ms) { return Duration(std::chrono::milliseconds(ms)); }
    static Duration nanoseconds(long long ns) { return Duration(std::chrono::nanoseconds(ns)); }
    static Duration maximum();

private:
    Backend _duration;
};
}
