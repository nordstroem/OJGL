#pragma once

#include "SyncChannel.h"
#include "V2MPlayer.h"
#include "utility/OJstd.h"
#include "utility\Timer.hpp"

namespace ojgl {

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
    void _initSync();
    unsigned char* _song;
    ojstd::shared_ptr<V2MPlayer> _player;
    Timepoint _startTime;
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
};

} //namespace ojgl
