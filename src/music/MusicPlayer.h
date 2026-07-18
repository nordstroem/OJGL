#pragma once

#include "SyncEvent.hpp"
#include "utility/Duration.h"
#include "utility/OJstd.h"

namespace ojgl {

class MusicPlayer {
public:
    virtual ~MusicPlayer() = default;

    virtual void play(Duration startTime) = 0;
    virtual void stop() = 0;

    virtual Duration elapsedTime() const = 0;

    virtual ojstd::vector<SyncEvent> popSyncEvents() = 0;
};

ojstd::shared_ptr<MusicPlayer> createSelectedPlayer();

}
