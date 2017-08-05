#pragma once

#include "syncchannel.h"
#include "v2mplayer.h"
#include <map>
#include <memory>

namespace ojgl {

class Music {

public:
    Music(unsigned char* song);
    ~Music();
    void play();
    void updateSync();
    std::map<int, SyncChannel> syncChannels;

private:
    void initSync();
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
};
} //namespace ojgl