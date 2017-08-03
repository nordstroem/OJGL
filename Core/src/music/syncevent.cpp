#include "SyncEvent.h"

SyncEvent::SyncEvent(int channel, int note, int velocity)
    : channel(channel)
    , note(note)
    , velocity(velocity)
{
}

SyncEvent::~SyncEvent()
{
}
