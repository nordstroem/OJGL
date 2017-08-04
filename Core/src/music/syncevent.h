#pragma once
class SyncEvent {
public:
    SyncEvent(int channel, int note, int velocity);
    ~SyncEvent();

    int channel, note, velocity;
};
