#pragma once

#include "SyncChannel.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

#ifdef OJGL_SYNTH_4KLANG
class FourKlangPlayer;
#else
class V2MPlayer;
#endif

class Music {

public:
    Music(const Music& other) = delete;
    Music& operator=(const Music& other) = delete;
    ~Music();

    static ojstd::shared_ptr<Music> instance();
    static void createInstance(const unsigned char* song, bool fixedTimestep);

    void play();
    void updateSync();
    void setTime(Duration time);
    void stop();
    Duration elapsedTime() const;
    ojstd::unordered_map<int, SyncChannel>& syncChannels();

private:
    explicit Music(const unsigned char* song, bool fixedTimestep);

private:
#ifdef OJGL_SYNTH_4KLANG
    ojstd::shared_ptr<FourKlangPlayer> _player;
#else
    ojstd::shared_ptr<V2MPlayer> _player;
#endif
    void _initSync();
#ifdef OJGL_SYNTH_4KLANG
    // 4klang renders on a background thread; audio start is deferred until the render finishes.
    void _tryStartAudio();
    bool _audioStarted = false;
    unsigned int _pendingStartMs = 0;
#endif
    const unsigned char* _song;
    Duration _syncOffset;
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
    int _currentFrame = 0;
    bool _fixedTimestep = false;
};

} //namespace ojgl
