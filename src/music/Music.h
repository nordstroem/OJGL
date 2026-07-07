#pragma once

#include "SyncChannel.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

#ifdef OJGL_SYNTH_CLINKSTER
class ClinksterPlayer;
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
#ifdef OJGL_SYNTH_CLINKSTER
    ojstd::shared_ptr<ClinksterPlayer> _player;
#else
    ojstd::shared_ptr<V2MPlayer> _player;
#endif
    void _initSync();
    const unsigned char* _song;
    Duration _syncOffset = Duration::milliseconds(0);
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
    int _currentFrame = 0;
    bool _fixedTimestep = false;
};

} //namespace ojgl
