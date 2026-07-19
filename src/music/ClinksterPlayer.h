#pragma once

#include "MusicPlayer.h"
#include "SyncEvent.hpp"
#include "utility/OJstd.h"
#ifdef _DEBUG
#include "AudioOutput.h"
#endif

namespace ojgl {

class ClinksterPlayer final : public MusicPlayer {
public:
    ClinksterPlayer();
    ClinksterPlayer(const ClinksterPlayer&) = delete;
    ClinksterPlayer& operator=(const ClinksterPlayer&) = delete;
    ~ClinksterPlayer() override;

    void play(Duration startTime) override;
    void stop() override;
    Duration elapsedTime() const override;

    ojstd::vector<SyncEvent> popSyncEvents() override;

private:

#ifdef _DEBUG
    // (Re)starts playback from startMs into the song via DirectSound.
    void startAudio(unsigned long startMs, void* hWnd);
    void stopAudio();
    // Play cursor in milliseconds from the start of the song.
    long elapsedMilliseconds() const;

    ojstd::shared_ptr<AudioOutput> _audio;
#endif

    bool  _renderDone = false;
#ifndef _DEBUG
    bool _started = false; // Clinkster's native waveOut player is start-once
#endif
};

} // namespace ojgl
