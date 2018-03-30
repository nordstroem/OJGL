#pragma once

#include "utility\Timer.hpp"

namespace ojgl {

struct SyncEvent {

    SyncEvent(int channel, int note, int velocity, Duration time)
        : channel(channel)
        , note(note)
        , velocity(velocity)
        , time(time){};
    ~SyncEvent() = default;

    const int channel;
    const int note;
    const int velocity;
    const Duration time;
};
} //namespace ojgl
