#pragma once

#include "AudioOutput.h"
#include "MusicPlayer.h"
#include "SyncEvent.hpp"
#include "utility/OJstd.h"

namespace ojgl {

// Clinkster (Blueberry/Loonies 4k softsynth) backend. Unlike V2, the song is baked into
// build-time assembly and rendered up front into the static Clinkster_MusicBuffer[]; playback,
// seeking and timing all go through our AudioOutput DirectSound wrapper (Clinkster's built-in
// waveOut player cannot reposition).
class ClinksterPlayer final : public MusicPlayer {
public:
    ClinksterPlayer();
    ClinksterPlayer(const ClinksterPlayer&) = delete;
    ClinksterPlayer& operator=(const ClinksterPlayer&) = delete;
    ~ClinksterPlayer() override;

    // MusicPlayer interface. play() renders the whole song up front on the first call (a few
    // seconds), then (re)starts DirectSound playback from startTime. elapsedTime() is the
    // absolute play cursor, so no offset accounting is needed.
    void play(Duration startTime) override;
    void stop() override;
    Duration elapsedTime() const override;

    // Scans the baked Clinkster_NoteTiming table (filled during Clinkster_GenerateMusic) and
    // synthesizes one SyncEvent per detected note trigger, per track. The result is meant to be
    // consumed once by Music::_initSync().
    ojstd::vector<SyncEvent> popSyncEvents() override;

private:
    static unsigned long __stdcall renderThreadProc(void* self);

    // Kicks off the full-song render on a background thread (it takes a few seconds).
    void beginRender();
    // True once the background render has finished and the buffer is ready to play.
    bool renderDone() const;
    // (Re)starts playback from startMs into the song via DirectSound.
    void startAudio(unsigned long startMs, void* hWnd);
    void stopAudio();
    // Play cursor in milliseconds from the start of the song.
    long elapsedMilliseconds() const;

    ojstd::shared_ptr<AudioOutput> _audio;
    void* _renderThread = nullptr; // HANDLE
    volatile long _renderDone = 0;
};

} // namespace ojgl
