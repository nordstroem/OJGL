#pragma once

#include "SyncChannel.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

class V2MPlayer;

class Music {

public:
    explicit Music(unsigned char* song);
    ~Music();

    void play();
    void updateSync();
    void setTime(Duration time);
    void stop();

    ojstd::unordered_map<int, SyncChannel>& syncChannels();

private:
    ojstd::shared_ptr<V2MPlayer> _player;
    void _initSync();
    unsigned char* _song;
    Timepoint _startTime;
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
};

} //namespace ojgl
