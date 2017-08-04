#pragma once

namespace ojgl {

struct SyncEvent {

    SyncEvent(int channel, int note, int velocity)
        : channel(channel)
        , note(note)
        , velocity(velocity){};
    ~SyncEvent(){};

    const int channel;
    const int note;
    const int velocity;
};
} //namespace ojgl