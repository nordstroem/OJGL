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

    int channel;
    int note;
    int velocity;
    Duration time;
};
} //namespace ojgl
