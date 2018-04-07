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
    void setTime(Duration time);
    void stop();

    std::unordered_map<int, SyncChannel>& syncChannels();

private:
    void _initSync();
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
    Timepoint _startTime;
    std::unordered_map<int, SyncChannel> _syncChannels;
};

} //namespace ojgl
