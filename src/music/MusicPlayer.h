#pragma once

#include "SyncEvent.hpp"
#include "utility/Duration.h"
#include "utility/OJstd.h"

namespace ojgl {

// Backend-agnostic playback interface. V2MPlayer and ClinksterPlayer each implement this, so
// Music can drive playback without knowing which synth is compiled in. Everything a backend
// does differently -- one-time init, seeking, timing offsets, sync-event extraction -- lives
// behind these methods.
class MusicPlayer {
public:
    virtual ~MusicPlayer() = default;

    // (Re)start / seek playback to startTime. Performs one-time init on the first call, and
    // leaves the backend's sync events available via popSyncEvents().
    virtual void play(Duration startTime) = 0;
    virtual void stop() = 0;

    // Absolute elapsed time from song start (each backend owns its own offset accounting).
    virtual Duration elapsedTime() const = 0;

    virtual ojstd::vector<SyncEvent> popSyncEvents() = 0;
};

// Constructs the player for the backend compiled into this binary, injecting the embedded song
// for V2. Returns nullptr when no song is embedded (=> the demo runs without music). Defined in
// the one player .cpp that CMake compiles for the selected SYNTH.
ojstd::shared_ptr<MusicPlayer> createSelectedPlayer();

}
