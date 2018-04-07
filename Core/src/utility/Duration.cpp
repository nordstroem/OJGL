#include "Duration.h"

namespace ojgl {

Duration& Duration::operator-=(const Duration& other)
{
    this->_duration -= other._duration;
    return *this;
}

Duration& Duration::operator+=(const Duration& other)
{
    this->_duration += other._duration;
    return *this;
}

Duration Duration::maximum()
{
    return Duration(std::chrono::nanoseconds(std::numeric_limits<long long>::max()));
}

std::ostream& operator<<(std::ostream& left, const Duration& right)
{
    return left << right.toMilliseconds() << "ms";
}
}
