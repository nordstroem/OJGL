#pragma once

#include "AudioOutput.h"
#include "SyncEvent.hpp"
#include "utility/OJstd.h"

namespace ojgl {

// Clinkster (Blueberry/Loonies 4k softsynth) backend. Unlike V2, the song is baked into
// build-time assembly and rendered up front into the static Clinkster_MusicBuffer[]; playback,
// seeking and timing all go through our AudioOutput DirectSound wrapper (Clinkster's built-in
// waveOut player cannot reposition). Beat-sync is deferred for this first pass, so this backend
// exposes no sync events yet (audio-only).
class ClinksterPlayer {
public:
    ClinksterPlayer();
    ClinksterPlayer(const ClinksterPlayer&) = delete;
    ClinksterPlayer& operator=(const ClinksterPlayer&) = delete;
    ~ClinksterPlayer();

    // Kicks off the full-song render on a background thread (it takes a few seconds).
    void beginRender();
    // True once the background render has finished and the buffer is ready to play.
    bool renderDone() const;

    // (Re)starts playback from startMs into the song via DirectSound.
    void startAudio(unsigned long startMs, void* hWnd);
    void stopAudio();

    // Play cursor in milliseconds from the start of the song.
    long elapsedMilliseconds() const;

    // Scans the baked Clinkster_NoteTiming table (filled during Clinkster_GenerateMusic) and
    // synthesizes one SyncEvent per detected note trigger, per track. Must be called once after
    // renderDone() and before startAudio(); the result is meant to be consumed once by
    // Music::_initSync().
    ojstd::vector<SyncEvent> popSyncEvents();

private:
    static unsigned long __stdcall renderThreadProc(void* self);

    ojstd::shared_ptr<AudioOutput> _audio;
    void* _renderThread = nullptr; // HANDLE
    volatile long _renderDone = 0;
};

} // namespace ojgl
