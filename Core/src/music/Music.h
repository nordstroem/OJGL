#pragma once

#include "SyncChannel.h"
#include "V2MPlayer.h"
#include "utility\Timer.hpp"
#include <unordered_map>

namespace ojgl {

class Music {

public:
    explicit Music(unsigned char* song);
    ~Music();
    void play();
    void updateSync();
    std::unordered_map<int, SyncChannel> syncChannels;
    void setTime(Duration time);
    void stop();

private:
    void _initSync();
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
    Timepoint _startTime;
};

} //namespace ojgl
