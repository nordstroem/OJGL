#include "Duration.h"
#include <limits>

#ifdef max
#undef max
#endif

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
    return Duration(std::numeric_limits<long>::max());
}

//std::ostream& operator<<(std::ostream& left, const Duration& right)
//{
//    return left << right.toMilliseconds() << "ms";
//}
}
