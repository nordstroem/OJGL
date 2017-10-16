#pragma once

#include "SyncChannel.h"
#include "V2MPlayer.h"
#include "utility\Timer.hpp"
#include <map>

namespace ojgl {

class Music {

public:
    Music(unsigned char* song);
    ~Music();
    void play();
    void updateSync();
    std::map<int, SyncChannel> syncChannels;
    void restart();

private:
    void initSync();
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
    ojgl::timer::Timer _songTimer;
};
} //namespace ojgl