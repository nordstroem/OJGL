#include "Timepoint.h"
#include "Duration.h"

namespace ojgl {

Timepoint& Timepoint::operator+=(const Duration& other)
{
    this->_timepoint += other.backend();
    return *this;
}

Timepoint& Timepoint::operator-=(const Duration& other)
{
    this->_timepoint -= other.backend();
    return *this;
}
}
