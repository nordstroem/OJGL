#pragma once

#include "MusicPlayer.h"
#include "SyncEvent.hpp"
#include "utility/OJstd.h"
#ifdef _DEBUG
#include "AudioOutput.h"
#endif

namespace ojgl {

// Clinkster (Blueberry/Loonies 4k softsynth) backend. The song is baked into build-time assembly
// and rendered up front into the static Clinkster_MusicBuffer[]. Playback differs by config:
//   * Release/CrinklerRelease use Clinkster's native waveOut player (Clinkster_StartMusic /
//     Clinkster_GetPosition). Its timer applies CLINKSTER_TIMER_OFFSET to compensate for display
//     latency, which is the tuning the reference engine ships. It cannot seek, but release never
//     does (all scrubbing/restart is _DEBUG-only).
//   * Debug/OptimizedDebug route through our AudioOutput DirectSound wrapper instead, because the
//     native player cannot reposition and the timeline scrubbing / restart controls need to seek.
class ClinksterPlayer final : public MusicPlayer {
public:
    ClinksterPlayer();
    ClinksterPlayer(const ClinksterPlayer&) = delete;
    ClinksterPlayer& operator=(const ClinksterPlayer&) = delete;
    ~ClinksterPlayer() override;

    // MusicPlayer interface. play() renders the whole song up front on the first call (a few
    // seconds), then starts playback. In release startTime is ignored (the native player only
    // plays from the beginning); in debug it (re)starts DirectSound from startTime.
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

#ifdef _DEBUG
    // (Re)starts playback from startMs into the song via DirectSound.
    void startAudio(unsigned long startMs, void* hWnd);
    void stopAudio();
    // Play cursor in milliseconds from the start of the song.
    long elapsedMilliseconds() const;

    ojstd::shared_ptr<AudioOutput> _audio;
#endif

    void* _renderThread = nullptr; // HANDLE
    volatile long _renderDone = 0;
#ifndef _DEBUG
    bool _started = false; // Clinkster's native waveOut player is start-once
#endif
};

} // namespace ojgl
