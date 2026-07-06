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

    // Audio-only backend: no beat-sync events. Kept to mirror V2MPlayer so Music::_initSync
    // builds an empty channel set (see the deferred beat-sync follow-up).
    ojstd::vector<SyncEvent> popSyncEvents() { return {}; }

private:
    static unsigned long __stdcall renderThreadProc(void* self);

    ojstd::shared_ptr<AudioOutput> _audio;
    void* _renderThread = nullptr; // HANDLE
    volatile long _renderDone = 0;
};

} // namespace ojgl
