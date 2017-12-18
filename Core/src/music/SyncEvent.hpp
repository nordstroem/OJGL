#pragma once

namespace ojgl {

struct SyncEvent {

    SyncEvent(int channel, int note, int velocity, timer::ms_t time)
        : channel(channel)
        , note(note)
        , velocity(velocity)
        , time(time){};
    ~SyncEvent() = default;

    const int channel;
    const int note;
    const int velocity;
    const timer::ms_t time;
};
} //namespace ojgl