#pragma once

#include "../../fakelibs/fakelib.h"
#include "..\utility\Timer.hpp"
#include "SyncChannel.h"
#include "V2MPlayer.h"

namespace ojgl {

class Music {

public:
    explicit Music(unsigned char* song);
    ~Music();

    void play();
    void updateSync();
    void setTime(Duration time);
    void stop();

    fl::unordered_map<int, SyncChannel>& syncChannels();

private:
    void _initSync();
    unsigned char* _song;
    fl::shared_ptr<V2MPlayer> _player;
    Timepoint _startTime;
    fl::unordered_map<int, SyncChannel> _syncChannels;
};

} //namespace ojgl
