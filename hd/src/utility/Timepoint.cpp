#include "Timepoint.h"
#include "Duration.h"

namespace ojgl {

Timepoint& Timepoint::operator+=(const Duration& other)
{
    this->_timepoint += other._duration;
    return *this;
}

Timepoint& Timepoint::operator-=(const Duration& other)
{
    this->_timepoint -= other._duration;
    return *this;
}
}
