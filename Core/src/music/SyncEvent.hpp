#pragma once

namespace ojgl {

struct SyncEvent {

    SyncEvent(int channel, int note, int velocity, Milliseconds time)
        : channel(channel)
        , note(note)
        , velocity(velocity)
        , time(time){};
    ~SyncEvent() = default;

    const int channel;
    const int note;
    const int velocity;
    const Milliseconds time;
};
} //namespace ojgl
