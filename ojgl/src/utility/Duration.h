#pragma once

namespace ojgl {

class Duration {

    friend class Timepoint;

public:
    Duration(long duration)
        : _duration(duration)
    {
    }

    Duration() = default;

    template <typename T = float>
    T toSeconds() const { return static_cast<T>(this->_duration) / 1000; }

    template <typename T = float>
    T toMilliseconds() const { return static_cast<T>(this->_duration); }

    //  template <typename T = float>
    //  T toNanoseconds() const { return std::chrono::duration_cast<std::chrono::duration<T, std::nano>>(_duration).count(); }

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

    //friend std::ostream& operator<<(std::ostream& left, const Duration& right);

public:
    static Duration seconds(long s) { return Duration(s * 1000); }
    static Duration milliseconds(long ms) { return Duration(ms); }
    //static Duration nanoseconds(long long ns) { return Duration(std::chrono::nanoseconds(ns)); }
    static Duration maximum();

private:
    long _duration;
};
}
